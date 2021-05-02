from datetime import datetime


class Sprint:
    def __init__(self, sprint_dict):
        self.id = sprint_dict['id']
        self.state = sprint_dict['state']
        self.name = sprint_dict['name']
        self.start = datetime.fromisoformat(sprint_dict['startDate'][0:-1]) if 'startDate' in sprint_dict else None
        self.end = datetime.fromisoformat(sprint_dict['endDate'][0:-1]) if 'endDate' in sprint_dict else None
        self.complete = datetime.fromisoformat(sprint_dict['completeDate'][0:-1]) if 'completeDate' in sprint_dict else None
        self.goal = sprint_dict['goal']

    def finish(self):
        if self.complete is None:
            return self.end
        else:
            return self.complete


class Issue:
    def __init__(self, issue_dict, sprint: Sprint = None):
        self.sprint = sprint
        self.id = issue_dict['key']
        self.summary = issue_dict['fields']['summary']
        self.type = issue_dict['fields']['issuetype']['name']
        self.time_estimate = round(issue_dict['fields']['timeestimate'] / 3600, 2) if issue_dict['fields']['timeestimate'] else 0
        self.time_spent = issue_dict['fields']['timetracking']['timeSpent'] if 'timeSpent' in issue_dict['fields']['timetracking'] else ''
        self.points = float(issue_dict['fields']['customfield_10027']) if 'customfield_10027' in issue_dict['fields'] and issue_dict['fields']['customfield_10027'] else 0.0
        self.status = issue_dict['fields']['status']['name']
        self.owner = issue_dict['fields']['assignee']['displayName']
        if issue_dict['fields']['resolutiondate']:
            self.resolution_at = datetime.fromisoformat(issue_dict['fields']['resolutiondate'][0:-5])
        if 'worklog' in issue_dict['fields']:
            self.work_logs = [WorkLog(work_log_dict) for work_log_dict in issue_dict['fields']['worklog']['worklogs']]
        else:
            self.work_logs = []

    def is_done(self):
        if self.sprint is None or self.status != 'Done':
            return False
        if self.resolution_at < self.sprint.start or self.resolution_at > self.sprint.finish():
            return False
        return True


class WorkLog:
    def __init__(self, work_log_dict):
        self.reporter = work_log_dict['author']['displayName']
        self.created_at = datetime.fromisoformat(work_log_dict['created'][0:-5])
        self.updated_at = datetime.fromisoformat(work_log_dict['updated'][0:-5]) if 'updated' in work_log_dict else None
        self.started_at = datetime.fromisoformat(work_log_dict['started'][0:-5]) if 'started' in work_log_dict else None
        self.time_spent_seconds = work_log_dict['timeSpentSeconds'] if 'timeSpentSeconds' in work_log_dict else 0.0


class PersonalPerformance:
    """
    統計一位團隊成員在一次衝刺內的表現績效.
    """
    def __init__(self, owner, stories, tasks, bugs, hours):
        self.owner = owner
        self.stories = stories
        self.tasks = tasks
        self.bugs = bugs
        self.hours = hours
        self.actual_stories = 0
        self.actual_tasks = 0
        self.actual_bugs = 0
        self.actual_hours = 0

    def add_issue(self, issue: Issue):
        self.__add_issue_count(issue)
        self.__add_issue_done(issue)
        self.hours += issue.time_estimate

    def __add_issue_count(self, issue: Issue):
        if issue.type == 'Story':
            self.stories += 1
        elif issue.type == 'Task':
            self.tasks += 1
        else:
            self.bugs += 1

    def __add_issue_done(self, issue: Issue):
        if not issue.is_done():
            return
        if issue.type == 'Story':
            self.actual_stories += 1
        elif issue.type == 'Task':
            self.actual_tasks += 1
        else:
            self.actual_bugs += 1

    def add_work_hours(self, work_log: WorkLog):
        self.actual_hours += round(work_log.time_spent_seconds / 3600, 2)


if __name__ == '__main__':
    date_str = '2020-08-05T05:30:15.372Z'
    date_format = '%Y-%m-%dT%H:%M:%S.%Z'
    d1 = datetime.fromisoformat(date_str[0:-1])

    resolution_date = '2020-08-24T08:57:20.300+0800'
    d2 = datetime.fromisoformat(resolution_date[0:-5])
    print(d1 < d2)
