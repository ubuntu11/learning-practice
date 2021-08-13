import json
from sprint_stats.model import Issue, IssueType


def test_issue_type_enum():
    assert IssueType.STORY == IssueType['Story'.upper()]
    assert IssueType.TASK == IssueType['Task'.upper()]
    assert IssueType.SUB_TASK == IssueType['Sub-task'.upper().replace('-','_')]
    assert IssueType.BUG == IssueType['Bug'.upper()]


def test_read_json_data():
    f = open('test_data.json',)
    data = json.load(f)
    assert len(data['issues']) == 7

    issue_dict = {issue['key'] : issue for issue in data['issues']}
    issue = Issue(issue_dict['SOM-57'], 'customfield_10027')

    assert issue.id == 'SOM-57'
    assert issue.summary == '身為設備管理員，我希望能建立及管理設備描述檔，以方便我可以套用至設備'
    assert issue.type == IssueType.STORY
    assert issue.is_story()
    assert issue.time_estimate == 2.0
    assert issue.time_spent == 0.0
    assert issue.points == 5
    f.close()
