import os
from flask import Flask
from flask_bootstrap import Bootstrap
from .controller import frontend
from werkzeug.routing import BaseConverter


class IntListConverter(BaseConverter):
    """
    See https://stackoverflow.com/questions/35437180/capture-a-list-of-integers-with-a-flask-route .
    """
    regex = r'\d+(?:,\d+)*,?'

    def to_python(self, value):
        return [int(x) for x in value.split(',')]

    def to_url(self, value):
        return ','.join(str(x) for x in value)


def create_app(test_config=None):
    app = Flask(__name__, static_url_path='', static_folder='static', template_folder='template',
                instance_relative_config=True)

    if test_config is None:
        # load the instance config, if it exists, when not testing
        app.config.from_pyfile('config.py', silent=True)
    else:
        # load the test config if passed in
        app.config.from_mapping(test_config)

    try:
        os.makedirs(app.instance_path)
    except OSError:
        pass

    Bootstrap(app)

    app.register_blueprint(frontend)
    app.url_map.converters['sprint_id_list'] = IntListConverter

    return app
