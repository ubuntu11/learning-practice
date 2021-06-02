FROM python:3.9-alpine

ARG config_file=config.py
ENV FLASK_ENV development
ENV FLASK_APP sprint_stats

RUN mkdir /app
RUN mkdir /app/instance
COPY sprint_stats /app/sprint_stats
COPY instance/$config_file /app/instance/config.py
WORKDIR /app
 
RUN pip install --upgrade pip
COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

EXPOSE 5000

ENTRYPOINT ["flask", "run", "--host=0.0.0.0"]
