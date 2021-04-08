from flask import Flask
from flask_bootstrap import Bootstrap
from .frontend import frontend

def create_app():
  app = Flask(__name__, static_url_path='',
            static_folder='static',
            template_folder='template')
  bootstrap = Bootstrap(app)

  app.register_blueprint(frontend)

  return app
