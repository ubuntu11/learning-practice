from flask import Blueprint, render_template, flash, redirect, url_for
from sprint_stats.main import read_csv_file

frontend = Blueprint('frontend', __name__)

@frontend.route('/')
def index():
    issue_performances = read_csv_file('aiot_5_issue_performance.csv')
    issues = read_csv_file('aiot_5_issues.csv')
    works = read_csv_file('aiot_5_works.csv')
    hours_by_day = read_csv_file('aiot_5_hours_by_day.csv')
    hours_total = read_csv_file('aiot_5_hours_total.csv')
    hours_record = __add_hours_total(read_csv_file('aiot_5_hours_record.csv'))

    return render_template('index.html', issues=issues, works=works, issue_performances=issue_performances, hours_by_day=hours_by_day[0],
      hours_total=hours_total[0], hours_record=hours_record)


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
