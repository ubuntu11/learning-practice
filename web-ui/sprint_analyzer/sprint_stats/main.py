import csv
import os

def read_csv_file(filename):
    """

    :param filename:
    :return: a list containing dicts
    """
    basedir = os.path.abspath(os.path.dirname(__file__))
    data_file = os.path.join(basedir, 'static/data/', filename)
    with open(data_file) as csvfile:
        rows = csv.DictReader(csvfile)
        issue_list = [row for row in rows]
    return issue_list
