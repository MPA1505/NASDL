from flask import Flask, jsonify, request
from pymongo import MongoClient
from flask_jwt_extended import JWTManager, jwt_required, create_access_token, get_jwt
from werkzeug.security import generate_password_hash, check_password_hash
from bson import ObjectId
import requests
import os
from datetime import timedelta
from flask_cors import CORS

app = Flask(__name__)
client = MongoClient(os.environ.get("MONGO_URI"))
db = client['usersdb']
usersCollection = db['users']
app.config['JWT_SECRET_KEY'] = 'super-secret'
jwt = JWTManager(app)
app.config['JWT_ACCESS_TOKEN_EXPIRES'] = timedelta(hours=4)
CORS(app)

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
    # Placeholder for open door logic
    return jsonify({"message": "Door opened successfully"}), 200

@app.route('/heartbeat', methods=['POST'])
@jwt_required()
def heartbeat():
    # Placeholder for heartbeat logic
    return jsonify({"message": "Heartbeat received"}), 200

initialize_user()

if __name__ == "__main__":
    app.run(debug=True)