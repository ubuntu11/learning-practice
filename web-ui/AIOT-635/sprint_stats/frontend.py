from flask import Blueprint, render_template, flash, redirect, url_for
from sprint_stats.main import read_csv_file

frontend = Blueprint('frontend', __name__)

@frontend.route('/')
def index():
    issue_performances = read_csv_file('aiot_24_issue_performance.csv')
    issues = read_csv_file('aiot_24_issues.csv')
    works = read_csv_file('aiot_24_works.csv')
    hours_by_day = read_csv_file('aiot_24_hours_by_day.csv')
    hours_total = read_csv_file('aiot_24_hours_total.csv')

    legend = '團隊回報工時'
    return render_template('index.html', issues=issues, works=works, issue_performances=issue_performances,
                           hours_by_day=hours_by_day[0], hours_total=hours_total[0], legend=legend)
