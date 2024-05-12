from flask import Flask, jsonify, request
from pymongo import MongoClient
from flask_jwt_extended import JWTManager, jwt_required, create_access_token, get_jwt
from werkzeug.security import generate_password_hash, check_password_hash
from bson import ObjectId
import os
import paho.mqtt.client as mqtt
from datetime import timedelta
from flask_cors import CORS
from flask_socketio import SocketIO, emit
import time

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")
client = MongoClient(os.environ.get("MONGO_URI"))
db = client['usersdb']
usersCollection = db['users']
app.config['JWT_SECRET_KEY'] = 'super-secret'
jwt = JWTManager(app)
app.config['JWT_ACCESS_TOKEN_EXPIRES'] = timedelta(hours=4)
CORS(app)

# MQTT Settings
mqtt_client = mqtt.Client()
mqtt_broker = os.environ.get("MQTT_BROKER", "mosquitto")
mqtt_port = int(os.environ.get("MQTT_PORT", 1883))
mqtt_open_topic = os.environ.get("MQTT_OPEN_TOPIC", "home/open_door")
mqtt_response_topic = os.environ.get("MQTT_RESPONSE_TOPIC", "home/response_door")


def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    if rc == 0:
        client.subscribe(mqtt_response_topic)
        print("Subscribed to MQTT topics")
    else:
        print("Failed to connect, return code %d\n", rc)

def on_message(client, userdata, msg):
    try:
        print(f"Received message '{msg.payload.decode()}' on topic '{msg.topic}'")
        decoded_message = msg.payload.decode()
        socketio.emit('door_status', {'data': decoded_message})
        print("Message emitted to socket clients")
    except Exception as e:
        print(f"Error in on_message: {e}")


mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

def connect_mqtt():
    try:
        mqtt_client.connect(mqtt_broker, mqtt_port, 60)
        mqtt_client.loop_start()  
        print("MQTT client connected")
    except Exception as e:
        print(f"Failed to connect to MQTT Broker: {e}")

connect_mqtt()

@jwt.user_identity_loader
def user_identity_lookup(user):
    return str(user["_id"])

@jwt.user_lookup_loader
def user_lookup_callback(_jwt_header, jwt_data):
    identity = jwt_data["sub"]
    return usersCollection.find_one({"_id": ObjectId(identity)})

# User initialization
def initialize_user():
    username = os.environ.get("USERNAME")
    password = os.environ.get("PASSWORD")

    if usersCollection.find_one({"username": username}):
        return

    hashed_password = generate_password_hash(password, method="sha512")
    usersCollection.insert_one({
        "username": username,
        "password": hashed_password
    })

@app.route('/')
def index():
    return 'Welcome to the backend API!'

# Login endpoint
@app.route("/login", methods=["POST"])
def login():
    username = request.json.get("username", None)
    password = request.json.get("password", None)

    user = usersCollection.find_one({"username": username})
    if user and check_password_hash(user["password"], password):
        access_token = create_access_token(identity=user)
        return jsonify({"access_token": access_token}), 200

    return jsonify({"msg": "Invalid username or password"}), 401

@app.route('/open-door', methods=['POST'])
@jwt_required()
def open_door():
    timestamp = request.json.get("timestamp", None)
    if timestamp is None:
        return jsonify({"error": "Timestamp required"}), 400
    payload = f"unlock,{timestamp}"
    mqtt_client.publish(mqtt_open_topic, payload)
    return jsonify({"message": "Unlock command sent", "timestamp": timestamp}), 200


@app.route('/heartbeat', methods=['POST'])
@jwt_required()
def heartbeat():
    # Placeholder for heartbeat logic
    return jsonify({"message": "Heartbeat received"}), 200

initialize_user()

if __name__ == "__main__":
    socketio.run(app, debug=True)