from flask import Blueprint, render_template, flash, redirect, url_for
from sprint_stats.main import read_csv_file

frontend = Blueprint('frontend', __name__)

@frontend.route('/')
def index():
    issues = read_csv_file('aiot_24_issues.csv')
    works = read_csv_file('aiot_24_works.csv')
    legend = 'Monthly Data'
    labels = ["January", "February", "March", "April", "May", "June", "July", "August"]
    values = [10, 9, 8, 7, 6, 4, 7, 8]
    return render_template('index.html', issues = issues, works=works, values=values, labels=labels, legend=legend)
