from flask import Blueprint, render_template, flash, redirect, url_for
from sprint_stats.main import read_csv_file
from sprint_stats.service import load_sprint_issues, get_sprint

frontend = Blueprint('frontend', __name__)


@frontend.route('/')
def index():
    sprint = get_sprint(120)
    issue_performances = read_csv_file('aiot_5_issue_performance.csv')
    issues, hours_by_day, hours_total = load_sprint_issues(sprint)
    works = read_csv_file('aiot_5_works.csv')
    hours_record = __add_hours_total(read_csv_file('aiot_5_hours_record.csv'))

    return render_template('index.html', sprint=sprint, issues=issues, works=works, issue_performances=issue_performances,
                           hours_by_day=hours_by_day, hours_total=hours_total, hours_record=hours_record)


def __add_hours_total(hours_record: dict):
    plan_hours_total = 0
    actual_hours_total = 0
    for h in hours_record:
        plan_hours_total += float(h['plan'])
        actual_hours_total += float(h['actual'])

    for h in hours_record:
        h['plan_total'] = plan_hours_total
        h['actual_total'] = actual_hours_total
    return hours_record
