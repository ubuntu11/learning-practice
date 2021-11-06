import json
import pytest
from sprint_stats.model import Issue, IssueType
import os


@pytest.fixture
def som_sprint_128():
    here = os.path.dirname(os.path.abspath(__file__))
    f = open(os.path.join(here, 'test_data_sprint_128.json'), 'r')
    data = json.load(f)
    f.close()
    return data


@pytest.fixture
def som_sprint_127():
    here = os.path.dirname(os.path.abspath(__file__))
    f = open(os.path.join(here, 'test_data_sprint_127.json'), 'r')
    data = json.load(f)
    f.close()
    return data


@pytest.fixture
def som_sprint_126():
    here = os.path.dirname(os.path.abspath(__file__))
    f = open(os.path.join(here, 'test_data_sprint_126.json'), 'r')
    data = json.load(f)
    f.close()
    return data


def test_issue_type_enum():
    assert IssueType.STORY == IssueType['Story'.upper()]
    assert IssueType.TASK == IssueType['Task'.upper()]
    assert IssueType.SUB_TASK == IssueType['Sub-task'.upper().replace('-','_')]
    assert IssueType.BUG == IssueType['Bug'.upper()]


def test_sprint_128(som_sprint_128):
    assert len(som_sprint_128['issues']) == 7

    issue_dict = {issue['key'] : issue for issue in som_sprint_128['issues']}
    issue = Issue(issue_dict['SOM-57'], 'customfield_10027')

    assert issue.id == 'SOM-57'
    assert issue.summary == '身為設備管理員，我希望能建立及管理設備描述檔，以方便我可以套用至設備'
    assert issue.type == IssueType.STORY
    assert issue.is_story()
    assert issue.time_estimate == 2.0
    assert issue.time_spent == 0.0
    assert issue.points == 5


def test_sprint_126(som_sprint_126):
    assert len(som_sprint_126['issues']) == 8

    issue_dict = {issue['key'] : issue for issue in som_sprint_126['issues']}
    issue = Issue(issue_dict['SOM-50'], 'customfield_10027')

    assert issue.id == 'SOM-50'
    assert issue.summary == 'GKE部署的container出現與Hikari CP有關的警告訊息'
    assert issue.type == IssueType.BUG
    assert issue.is_bug()
    assert issue.time_estimate == 2.0
    assert issue.time_spent == 1.5
    assert issue.points == 0


def test_sprint_127(som_sprint_127):
    assert len(som_sprint_127['issues']) == 2

    issue_dict = {issue['key'] : issue for issue in som_sprint_127['issues']}
    issue = Issue(issue_dict['SRP-30'], 'customfield_10027')

    assert issue.id == 'SRP-30'
    assert issue.summary == '循軟體中心流程提出正式報價'
    assert issue.type == IssueType.TASK
    assert issue.is_task()
    assert issue.time_estimate == 3.0
    assert issue.time_spent == 1.0
    assert issue.points == 0