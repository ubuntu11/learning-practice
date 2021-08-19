# Introduction
使用Flask + Bootstrap，嘗試在團隊UI指定的引導下，以衝刺過程統計資料為主題，製做一個示範性網頁。

## Prepare Env

- Install Virtualenv
`
pip3 install virtualenv --user
`

- Create environment
`
virtualenv -p python3 venv
`

- Activate virtual env
`
source venv/bin/activate
`

## Install Dependencies

`
pip install -r requirements.txt
`

## Run

- Set environment variables
`
export FLASK_APP=sprint_stats
export FLASK_ENV=development
export JIRA_CREDENTIAL_BASE64=a2VuLmh1QGh3YWNvbS5jb206UlFoYzZJUnJ3MHNRVVRpTG9xdnY3QkZD
`
- Start Flask
`
flask run
`
- View Page
Open your browser and visit http://localhost:5000

## Build

Build docker image:
```
docker build --build-arg config_file=config_rd2.py -t aiothwacom/sprint-analyzer:rd2 .
```

## Reference

### Jira Board Number

|Team|Board Name|Board id|
|---|---|---|
|RD2|SOM board|8|
|RD2|SRP board|7|
|RD5|TCIS1 baord|5|