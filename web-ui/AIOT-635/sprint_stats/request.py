import requests
from operator import attrgetter
from flask import current_app
from sprint_stats.objects import Issue, Sprint, WorkLog


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
    sprint_url = f'{__base_url()}sprint/{sprint_id}'
    response = requests.get(sprint_url, headers=__request_header())
    return Sprint(response.json())


def load_sprint_issues(sprint_id: int):
    issue_url = f'{__base_url()}sprint/{sprint_id}/issue'
    response = requests.get(issue_url, headers=__request_header())
    issues_json = response.json()['issues']
    issue_unsorted = [Issue(issue_dict) for issue_dict in issues_json]
    return sorted(issue_unsorted, key=attrgetter('status', 'id'))


if __name__ == '__main__':
    sprint = get_sprint(104)
    print(sprint.goal)