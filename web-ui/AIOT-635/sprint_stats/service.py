from operator import attrgetter

import requests
from flask import current_app
import datetime
from sprint_stats.model import Issue, Sprint, PersonalPerformance, VirtualSprint
import os


def __config():
    """
    透過current_app取得目前所屬的flask application context中的config物件.把current_app.config直接定義成global attribute會發生
    RuntimeError: Working outside of application context.
    """
    return current_app.config


def __base_url(team_id: str) -> list[str]:
    return [f'https://{d}/' + __config()["API_PREFIX"] for d in __config()["JIRA_DOMAIN"][team_id]]


def __request_header():
    """
    :return: base64 string of username:password
    """
    return {'Authorization': f'Basic {os.environ["JIRA_CREDENTIAL_BASE64"]}'}


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


def load_sprint_issues(team_id: str, sprint_id_list: list[int]):
    """
    取得某個衝刺內的所有Issue.
    :param team_id: 研發團隊代號, ex: RD2
    :param sprint_id_list: 要進行統計分析的衝刺(as a list)
    :return: a list of Issue instances
    """
    sprints = [get_sprint(f'{__base_url(team_id)[i]}/sprint/{s_id}', s_id) for i, s_id in enumerate(sprint_id_list)]
    virtual_sprint = VirtualSprint(sprints)
    issue_unsorted = []
    issues_json_list = []
    story_point_col_name_dict = __config()['STORY_POINT_COL_NAME']
    for i, s in enumerate(virtual_sprint.sprints):
        issue_url = f'{__base_url(team_id)[i]}/sprint/{s.id}/issue'
        response = requests.get(issue_url, headers=__request_header())
        issues_json = response.json()['issues']
        for issue_json in issues_json:
            if issue_json['fields']['sprint'] is not None:
                issue_json['fields']['sprint']['id'] = s.id
            else:
                issue_json['fields']['sprint'] = {'id': s.id}
        # 由於PBI可能因為在前一個衝刺沒完成而搬到下一個衝刺，造成由Issue取得的sprint id與指定的sprint id不同,所以擴展透過API取得的
        # Issue json資料，把指定的sprint id放進去

        issues_json_list.extend(issues_json)

    issue_json_dict = {issue_json['key']: issue_json for issue_json in issues_json_list}
    for key in issue_json_dict:
        sprint_id = issue_json_dict[key]['fields']['sprint']['id']
        current_sprint = virtual_sprint.sprint_dict[sprint_id]
        project_id = issue_json_dict[key]['fields']['project']['id']
        sub_task_list = [Issue(issue_json_dict[sub_task['key']], story_point_col_name_dict[project_id], current_sprint) for sub_task in issue_json_dict[key]['fields']['subtasks']]
        issue_unsorted.append(Issue(issue_json_dict[key],
                                    story_point_col_name_dict[project_id],
                                    current_sprint,
                                    sub_task_list))

    # time_sheet_by_date_dict -> date :  commit_hours
    time_sheet_by_date_dict = __init_time_sheet_by_date_dict(virtual_sprint)
    # personal_performance_dict -> owner_name : PersonalPerformance instance
    personal_performance_dict = {}
    for issue in issue_unsorted:
        print(issue.sprint.start)
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
    for d in sorted([d for d in time_sheet_by_date_dict.keys()]):
        hours_by_date_list.append((d, round(time_sheet_by_date_dict[d] / 3600, 2)))

    hours_total_by_date_list = []
    today = datetime.date.today()
    for idx, date in enumerate(sorted([d for d in time_sheet_by_date_dict.keys()])):
        if datetime.date.fromisoformat(date) > today:
            continue

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
    delta = datetime.timedelta(days=1)
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
