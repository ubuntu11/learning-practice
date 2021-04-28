from operator import attrgetter

import requests
from flask import current_app

from sprint_stats.model import Issue, Sprint


def __config():
    """
    透過current_app取得目前所屬的flask application context中的config物件.把current_app.config直接定義成global attribute會發生
    RuntimeError: Working outside of application context.
    """
    return current_app.config


def __base_url():
    return  f'https://{__config()["JIRA_DOMAIN"]}/{__config()["API_PREFIX"]}/'


def __request_header():
    return  {'Authorization': f'Basic {__config()["JIRA_CREDENTIAL_BASE64"]}'}


def get_sprint(sprint_id: int):
    """
    取得某個衝刺.
    :param sprint_id: 衝刺id, ex: 104
    :return: an instance of Sprint class
    """
    sprint_url = f'{__base_url()}sprint/{sprint_id}'
    response = requests.get(sprint_url, headers=__request_header())
    return Sprint(response.json())


def load_sprint_issues(sprint: Sprint):
    """
    取得某個衝刺內的所有Issue.
    :param sprint: 某個衝刺
    :return: a list of Issue instances
    """
    issue_url = f'{__base_url()}sprint/{sprint.id}/issue'
    response = requests.get(issue_url, headers=__request_header())
    issues_json = response.json()['issues']
    issue_unsorted = [Issue(issue_dict) for issue_dict in issues_json]

    time_sheet_by_date_dict = {}
    for issue in issue_unsorted:
        for work_log in issue.work_logs:
            # 排除不在該次衝刺時間內的work log
            if work_log.started_at < sprint.start or work_log.started_at > sprint.end:
                continue
            if str(work_log.started_at.date()) in time_sheet_by_date_dict:
                time_sheet_by_date_dict[str(work_log.started_at.date())] += work_log.time_spent_seconds
            else:
                time_sheet_by_date_dict[str(work_log.started_at.date())] = work_log.time_spent_seconds

    hours_by_date_list = []
    for date in sorted([d for d in time_sheet_by_date_dict.keys()]):
        hours_by_date_list.append((date, round(time_sheet_by_date_dict[date] / 3600, 2)))

    hours_total_by_date_list = []
    for idx, date in enumerate(sorted([d for d in time_sheet_by_date_dict.keys()])):
        hours_total = 0.0
        for i in range(idx+1):
            hours_total += hours_by_date_list[i][1]
        hours_total_by_date_list.append((date, hours_total))

    return sorted(issue_unsorted, key=attrgetter('status', 'id')), hours_by_date_list, hours_total_by_date_list


if __name__ == '__main__':
    sprint = get_sprint(104)
    print(sprint.goal)
