from datetime import datetime


class Sprint:
    def __init__(self, sprint_dict):
        self.id = sprint_dict['id']
        self.state = sprint_dict['state']
        self.name = sprint_dict['name']
        self.start = datetime.fromisoformat(sprint_dict['startDate'][0:-1])
        self.end = datetime.fromisoformat(sprint_dict['completeDate'][0:-1])
        self.goal = sprint_dict['goal']


class Issue:
    def __init__(self, issue_dict):
        self.id = issue_dict['key']
        self.summary = issue_dict['fields']['summary']
        self.type = issue_dict['fields']['issuetype']['name']
        self.time_spent = issue_dict['fields']['timetracking']['timeSpent'] if 'timeSpent' in issue_dict['fields']['timetracking'] else ''
        self.points = float(issue_dict['fields']['customfield_10027']) if 'customfield_10027' in issue_dict['fields'] else 0.0
        self.status = issue_dict['fields']['status']['name']
        self.owner = issue_dict['fields']['assignee']['displayName']
        self.resolution_at = datetime.fromisoformat(issue_dict['fields']['resolutiondate'][0:-5])


if __name__ == '__main__':
    date_str = '2020-08-05T05:30:15.372Z'
    date_format = '%Y-%m-%dT%H:%M:%S.%Z'
    d1 = datetime.fromisoformat(date_str[0:-1])

    resolutiondate = '2020-08-24T08:57:20.300+0800'
    d2 = datetime.fromisoformat(resolutiondate[0:-5])
    print(d1 < d2)