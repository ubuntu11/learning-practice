import requests
from operator import attrgetter
from sprint_stats.objects import Issue, Sprint

base_url = 'https://innodiv-hwacom.atlassian.net/rest/agile/1.0/'


def __header():
    return {'Authorization' : 'Basic a2VuLmh1QGh3YWNvbS5jb206UlFoYzZJUnJ3MHNRVVRpTG9xdnY3QkZD'};


def get_sprint(sprint_id: int):
    sprint_url = f'{base_url}sprint/{sprint_id}'
    response = requests.get(sprint_url, headers=__header())
    return Sprint(response.json())


def load_sprint_issues(sprint_id: int):
    issue_url = f'{base_url}sprint/{sprint_id}/issue'
    response = requests.get(issue_url, headers=__header())
    issues_json = response.json()['issues']
    issue_unsorted = [Issue(issue_dict) for issue_dict in issues_json]
    return sorted(issue_unsorted, key=attrgetter('status', 'id'))


if __name__ == '__main__':
    sprint = get_sprint(104)
    print(sprint.goal)