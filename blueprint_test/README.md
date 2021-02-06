# First try of Flask Blueprint

See [How To Structure Large Flask Applications](https://www.digitalocean.com/community/tutorials/how-to-structure-large-flask-applications).
It is quite old (in 2014), so we need to adjust codes a little bit.

## Development history

### Python virtual environment and dependencies

```
sudo pip3 install virtualenv
mkdir LargeApp
mkdir LargeApp/app
mkdir LargeApp/app/templates
mkdir LargeApp/app/static
virtualenv -p python3 venv
source venv/bin/activate
pip install -r requirements.txt

touch LargeApp/run.py
touch LargeApp/config.py
touch LargeApp/app/__init__.py
```

### Create folder structures and files

```
vi LargeApp/run.py
vi LargeApp/config.py

mkdir LargeApp/app/mod_auth
mkdir LargeApp/app/templates/auth
touch LargeApp/app/mod_auth/__init__.py
touch LargeApp/app/mod_auth/controllers.py
touch LargeApp/app/mod_auth/models.py
touch LargeApp/app/mod_auth/forms.py
touch LargeApp/app/templates/auth/signin.html
touch LargeApp/app/templates/404.html

vi LargeApp/app/mod_auth/models.py
vi LargeApp/app/mod_auth/forms.py
vi LargeApp/app/mod_auth/controllers.py
vi LargeApp/app/__init__.py
vi LargeApp/app/templates/auth/signin.html
```

## How to run

In this folder (blueprint_test),

- create virtual environment if needed

  ```
  virtualenv -p python3 venv
  source venv/bin/activate
  pip install -r requirements.txt
  ```

- start Flask

  In application root folder (blueprint_test/LargeApp),

  ```
  python run.py
  ```

  then you can visit http://localhost:8080/auth/signin/
