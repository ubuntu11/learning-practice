from flask import Flask, jsonify, request, render_template

app = Flask(__name__)

stores = [
    {'name': 'Japan AV',
     'items': {
         'title': 'AWT-078 淫語中出しソープ63',
         'performers': '香苗レノン'
     }
     }
]


@app.route('/')
def home_page():
    return render_template('index.html')


@app.route('/stores')
def show_stores():
    return jsonify({'stores': stores})


@app.route('/stores/<string:name>')
def show_store(name):
    for store in stores:
        if store['name'] == name:
            return jsonify(store)
    return jsonify({'message': 'we can not find that store'})


@app.route('/stores', methods=['POST'])
def create_store(store_json):
    request_data = request.get_json()
    new_store = {'name': request_data['name'],
                 'items': []}

    stores.append(new_store)
    return jsonify(new_store)


app.run()
