FROM python:3.9-alpine

ARG config_file=config.py
ARG JIRA_CREDENTIAL_BASE64
ENV FLASK_ENV development
ENV FLASK_APP sprint_stats
ENV JIRA_CREDENTIAL_BASE64 ${JIRA_CREDENTIAL_BASE64}

RUN mkdir /app
RUN mkdir /app/instance
COPY sprint_stats /app/sprint_stats
COPY $config_file /app/instance/config.py
WORKDIR /app

RUN pip install --upgrade pip
COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

EXPOSE 5000

ENTRYPOINT ["flask", "run", "--host=0.0.0.0"]
