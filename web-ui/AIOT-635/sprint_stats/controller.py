from flask import Blueprint, render_template, flash, redirect, url_for, current_app
from sprint_stats.main import read_csv_file
from sprint_stats.service import load_sprint_issues, get_sprint
import re

frontend = Blueprint('frontend', __name__)


@frontend.route('/')
def home():
  return 'Welcome'


@frontend.route('/rd/<team_id>/sprints/<sprint_id_list>')
def index(team_id: int, sprint_id_list: str):
    # Make sure it is a list that only contains integers.
    if not re.match(r'^\d+(?:,\d+)*,?$', sprint_id_list):
        return "Please input a list of integers, split with ','"
    sprint_id_list = [int(x) for x in sprint_id_list.split(',')]
    sprints, issues, hours_by_day, hours_total, personal_performance_list = \
        load_sprint_issues(f'RD{team_id}', sprint_id_list)

    return render_template('index.html',
                           sprint=sprints[0],
                           issues=issues,
                           personal_performance_list=personal_performance_list,
                           hours_by_day=hours_by_day,
                           hours_total=hours_total)


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
