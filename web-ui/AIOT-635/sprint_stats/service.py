from operator import attrgetter

import requests
from flask import current_app
from datetime import timedelta
from sprint_stats.model import Issue, Sprint, PersonalPerformance, VirtualSprint


def __config():
    """
    透過current_app取得目前所屬的flask application context中的config物件.把current_app.config直接定義成global attribute會發生
    RuntimeError: Working outside of application context.
    """
    return current_app.config


def __base_url() -> list[str]:
    return [f'https://{d}/' + __config()["API_PREFIX"] for d in __config()["JIRA_DOMAIN"]]


def __request_header():
    return {'Authorization': f'Basic {__config()["JIRA_CREDENTIAL_BASE64"]}'}


def get_sprint(sprint_url: str, sprint_id: int):
    """
    取得某個衝刺.
    :param sprint_url: api base_url for retrieving sprint data
    :param sprint_id: 衝刺id, ex: 104
    :return: an instance of Sprint class
    """
    # sprint_url = f'{__base_url()}sprint/{sprint_id}'
    print('URL', sprint_url)
    response = requests.get(sprint_url, headers=__request_header())
    return Sprint(response.json())


def load_sprint_issues(sprint_id_list: list[int]):
    """
    取得某個衝刺內的所有Issue.
    :param sprints: 要進行統計分析的衝刺(as a list)
    :return: a list of Issue instances
    """
    sprints = [get_sprint(f'{__base_url()[i]}/sprint/{s_id}', s_id) for i, s_id in enumerate(sprint_id_list)]
    virtual_sprint = VirtualSprint(sprints)
    issue_unsorted = []
    for i, sprint in enumerate(virtual_sprint.sprints):
        issue_url = f'{__base_url()[i]}/sprint/{sprint.id}/issue'
        response = requests.get(issue_url, headers=__request_header())
        issues_json = response.json()['issues']
        issue_unsorted.extend([Issue(issue_dict, __config()['STORY_POINT_COL_NAME'][i], sprint)
                               for issue_dict in issues_json])

    # time_sheet_by_date_dict -> date :  commit_hours
    time_sheet_by_date_dict = __init_time_sheet_by_date_dict(virtual_sprint)
    # personal_performance_dict -> owner_name : PersonalPerformance instance
    personal_performance_dict = {}
    for issue in issue_unsorted:
        pp = __get_personal_performance(issue.owner, personal_performance_dict)
        pp.add_issue(issue)
        for work_log in issue.work_logs:
            # 排除不在該次衝刺時間內的work log
            if work_log.started_at < issue.sprint.start or work_log.started_at > issue.sprint.finish():
                continue
            pp = __get_personal_performance(work_log.reporter, personal_performance_dict)
            pp.add_work_hours(work_log)
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
        for i in range(idx + 1):
            hours_total += hours_by_date_list[i][1]
        hours_total_by_date_list.append((date, hours_total))

    personal_performance_list = [pp for pp in personal_performance_dict.values()]

    return sprints, sorted(issue_unsorted, key=attrgetter('id')), hours_by_date_list, hours_total_by_date_list\
        , sorted(personal_performance_list,  key=lambda p: p.owner)


def __init_time_sheet_by_date_dict(sprint:VirtualSprint) -> dict:
    time_sheet_by_date_dict = {}
    start_date = sprint.start.date()
    end_date = sprint.end.date()
    delta = timedelta(days=1)
    while start_date <= end_date:
        time_sheet_by_date_dict[str(start_date)] = 0
        start_date += delta
    return time_sheet_by_date_dict


def __get_personal_performance(name: str, personal_performance_dict: dict) -> PersonalPerformance:
    if name not in personal_performance_dict:
        pp = PersonalPerformance(name, 0, 0, 0, 0)
        personal_performance_dict[name] = pp

    return personal_performance_dict[name]


if __name__ == '__main__':
    sprint = get_sprint(104)
    print(sprint.goal)
