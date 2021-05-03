from datetime import datetime

DATE_FORMAT = '%Y-%m-%dT%H:%M:%S.%f%z'
DATE_FORMAT_WITH_OFFSET = '%Y-%m-%dT%H:%M:%S.%f%z'


class Sprint:
    def __init__(self, sprint_dict):
        self.id = sprint_dict['id']
        self.state = sprint_dict['state']
        self.name = sprint_dict['name']
        self.start = datetime.strptime(sprint_dict['startDate'], DATE_FORMAT) if 'startDate' in sprint_dict else None
        self.end = datetime.strptime(sprint_dict['endDate'], DATE_FORMAT) if 'endDate' in sprint_dict else None
        self.complete = datetime.strptime(sprint_dict['completeDate'], DATE_FORMAT) if 'completeDate' in sprint_dict else None
        self.goal = sprint_dict['goal']

    def finish(self):
        if self.complete is None:
            return self.end
        else:
            return self.complete


class VirtualSprint:
    def __init__(self, sprints):
        self.sprints = sprints
        self.start = sorted([s.start for s in self.sprints])[0]
        self.end = sorted([s.finish() for s in self.sprints], reverse=True)[0]


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
            self.resolution_at = datetime.strptime(issue_dict['fields']['resolutiondate'], DATE_FORMAT_WITH_OFFSET)
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
        self.created_at = datetime.strptime(work_log_dict['created'], DATE_FORMAT_WITH_OFFSET)
        self.updated_at = datetime.strptime(work_log_dict['updated'], DATE_FORMAT_WITH_OFFSET) if 'updated' in work_log_dict else None
        self.started_at = datetime.strptime(work_log_dict['started'], DATE_FORMAT_WITH_OFFSET) if 'started' in work_log_dict else None
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
    sprint_complete_date = '2021-05-03T06:00:12.960Z'
    date_format = '%Y-%m-%dT%H:%M:%S.%Z'
    #d1 = datetime.fromisoformat(date_str[0:-1])
    d1 = datetime.strptime(sprint_complete_date, "%Y-%m-%dT%H:%M:%S.%f%z")

    issue_resolution_date = '2021-05-03T08:57:59.294+0800'
    #d2 = datetime.fromisoformat(resolution_date[0:-5])
    d2 = datetime.strptime(issue_resolution_date, "%Y-%m-%dT%H:%M:%S.%f%z")
    print(d1 > d2)
