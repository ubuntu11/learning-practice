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
`
- Start Flask
`
flask run
`
- View Page
Open your browser and visit http://localhost:5000

