import json
import pytest
from sprint_stats.model import Issue, IssueType
import os


def som_sprint_issues(sprint_id):
    here = os.path.dirname(os.path.abspath(__file__))
    f = open(os.path.join(here, F'test_data_sprint_{sprint_id}.json'), 'r')
    data = json.load(f)
    f.close()
    return data


def test_issue_type_enum():
    assert IssueType.STORY == IssueType['Story'.upper()]
    assert IssueType.TASK == IssueType['Task'.upper()]
    assert IssueType.SUB_TASK == IssueType['Sub-task'.upper().replace('-', '_')]
    assert IssueType.BUG == IssueType['Bug'.upper()]


@pytest.mark.parametrize('sprint_id, issue_count, issue_id, issue_summary, issue_type, time_estimate, '
                         'time_spent, points, custom_field_name',
                         [(128, 7, 'SOM-57', '身為設備管理員，我希望能建立及管理設備描述檔，以方便我可以套用至設備',
                           IssueType.STORY, 2, 0, 5, 'customfield_10027'),
                          (126, 8, 'SOM-50', 'GKE部署的container出現與Hikari CP有關的警告訊息',
                           IssueType.BUG, 2, 1.5, 0, 'customfield_10027'),
                          (127, 2, 'SRP-30', '循軟體中心流程提出正式報價', IssueType.TASK, 3, 1, 0,
                           'customfield_10027'),
                          (135, 13, 'SOM-176', 'O&M 告警功能需求分析', IssueType.TASK, 36, 36, 0,
                           'customfield_10027')
                           ])
def test_sprints(sprint_id, issue_count, issue_id, issue_summary, issue_type, time_estimate,
                    time_spent, points, custom_field_name):
    issue_data = som_sprint_issues(sprint_id)
    assert len(issue_data['issues']) == issue_count

    issue_dict = {issue['key']: issue for issue in issue_data['issues']}
    issue = Issue(issue_dict[issue_id], custom_field_name)

    assert issue.id == issue_id
    assert issue.summary == issue_summary
    assert issue.type == issue_type
    assert issue.time_estimate == time_estimate
    assert issue.time_spent == time_spent
    assert issue.points == points


def test_sprint_135():
    issue_data = som_sprint_issues(135)
    issues = [Issue(issue, 'customfield_10027') for issue in issue_data['issues']]
    tasks = [issue for issue in issues if issue.is_task() or issue.is_sub_task()]
    stories = [issue for issue in issues if issue.is_story()]
    bugs = [issue for issue in issues if issue.is_bug()]
    assert len(stories) == 2
    assert len(tasks) == 11
    assert len(bugs) == 0