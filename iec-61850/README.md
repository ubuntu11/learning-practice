# About

根據台電電力交易平台通訊規範所開發的MMS Server - Client 通訊範例實作.
Clien收到Report後，會將資料儲存到資料庫中，接著以Grafana呈現這些模擬電力資料.

# Getting Started

Clone [libiec61850](https://github.com/mz-automation/libiec61850) repository from github.

## Prerequisites

- GCC
- Make
- sqlite
- Docker

## Installation

### sqlite

- install sqlite and its header files
```
sudo apt-get install sqlite3 libsqlite3-dev
```
- open database
```
sqlite3 taipower.db
```

- create tables

in sqlite session, copy and paste the content of schema.sql.

- check db schema

in sqlite session, execute ".schema". (note the leading dot)

### libiec61850

- build from source
```
cd ${LIBIEC61850_HOME}
make clean
make
```

### Grafana

- start Grafana container
```
docker run -d --user root --name=afc_grafana -v "$(pwd)":/sqlite_data -p 3000:3000 grafana/grafana
```

- install sqlite plugin: see [SQLite Plugin for Grafana](https://grafana.com/grafana/plugins/frser-sqlite-datasource/)

- create datasource (省略)

- create dashboard

滙入 華電AFC儲能一號場-XXX.json 檔

## Usage

### build from source

Edit Makefile and specify correct libiec61850 path, then:

```
make clean
make model
make
```

### simulate network traffics between MMS server and client

- start MMS Server
```
sudo ./server
```

- start MMS client
```
./client
```
