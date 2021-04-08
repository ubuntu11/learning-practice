from flask import Blueprint, render_template, flash, redirect, url_for

frontend = Blueprint('frontend', __name__)

@frontend.route('/')
def index():
    return render_template('index.html')
