import sys
from threading import Thread

from apscheduler.schedulers.blocking import BlockingScheduler
from apscheduler.triggers.combining import OrTrigger
from apscheduler.triggers.cron import CronTrigger
from pyjavaproperties3 import Properties

from access_control_server import start_server as start_access_tcp_server
from data_access import connect_to_db
from model import TMM70, S42
from read_modbus import read_meters, read_env, read_rio1, read_rio2, read_relay
from remote_control import em_control
from service import update_status, compute_modbus_reading


def main():
    """
    DC主程式.
    :return: None
    """
    print('start main thread')
    p = Properties()
    p.load(open('application.properties'))

    one_second_trigger = OrTrigger([CronTrigger(second='*/1')])
    five_second_trigger = OrTrigger([CronTrigger(second='*/5')])

    compute_modbus_reading(TMM70)
    compute_modbus_reading(S42)

    try:
        db_client = connect_to_db(f'{p["mongodb_ip"]}:{p["mongodb_port"]}', redundant=False)
        if not db_client:
            sys.exit('fail to connect to mongodb')

        # 控制線程
        control_thread = Thread(target=em_control, args=(db_client, p['em_modbus_ip'],
                                                         p['em_modbus_port']))
        control_thread.start()

        # 門禁系統讀取線程
        access_thread = Thread(target=start_access_tcp_server, args=(db_client, p['tcp_server_ip'],
                                                                     p['tcp_server_port']))
        access_thread.start()

        # 各副程式排程執行
        scheduler = BlockingScheduler({'apscheduler.job_defaults.max_instances': '50'})
        scheduler.add_job(update_status, five_second_trigger, args=(db_client,))
        scheduler.add_job(read_meters, five_second_trigger, args=(db_client, p['aux_meter_ip'],
                                                                  p['aux_meter_port'],
                                                                  p['lv_meter_ip'],
                                                                  p['lv_meter_port']))
        scheduler.add_job(read_env, five_second_trigger, args=(db_client, p['env_modbus_ip'],
                                                               p['env_modbus_port']))
        scheduler.add_job(read_rio1, one_second_trigger, args=(db_client, p['rio1_modbus_ip'],
                                                               p['rio1_modbus_port']))
        scheduler.add_job(read_rio2, one_second_trigger, args=(db_client, p['rio2_modbus_ip'],
                                                               p['rio2_modbus_port']))
        scheduler.add_job(read_relay, one_second_trigger,
                          args=(db_client, p['relay_modbus_ip'], p['relay_modbus_port']))
        scheduler.start()

    except Exception as meg:
        print('main error')
        print(meg)


if __name__ == '__main__':
    main()
