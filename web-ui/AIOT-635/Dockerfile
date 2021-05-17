FROM aiothwacom/python

RUN apk --no-cache add bash

RUN mkdir /app
RUN mkdir /app/instance/
RUN mkdir /app/sprint_stats/
COPY requirements.txt /app/
COPY instance/* /app/instance/
COPY sprint_stats/ /app/sprint_stats/

WORKDIR /app/
ENV FLASK_APP=sprint_stats
ENV FLASK_ENV=development

RUN python -m pip install --upgrade pip
RUN pip install -r requirements.txt
ENTRYPOINT ["flask", "run", "--host=0.0.0.0"]

