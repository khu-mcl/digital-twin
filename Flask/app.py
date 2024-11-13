from flask import Flask, jsonify
from engine_manager import EngineManager
from receiver_manager import ReceiverManager
from transmitter_manager import TransmitterManager
import logging
import signal
import sys

app = Flask(__name__)
logging.basicConfig(level=logging.DEBUG)

engine_manager = EngineManager()
receiver_manager = ReceiverManager(engine_manager)
transmitter_manager = TransmitterManager(engine_manager)

@app.route('/status', methods=['GET'])
def get_status():
    status = {
        "matlab_engine": engine_manager.get_status(),
        "receiver": receiver_manager.get_status(),
        "transmitter": transmitter_manager.get_status()
    }
    return jsonify(status), 200

@app.route('/control/<mode>/<action>', methods=['POST'])
def control(mode, action):
    try:
        if mode == 'receiver':
            return receiver_manager.control(action)
        elif mode == 'transmitter':
            return transmitter_manager.control(action)
        else:
            return 'Invalid mode', 400
    except Exception as e:
        logging.error(f"Error in {mode}/{action}: {str(e)}")
        return f"Error: {str(e)}", 500

def shutdown_server():
    logging.info("Shutting down server...")
    engine_manager.shutdown()
    receiver_manager.shutdown()
    transmitter_manager.shutdown()
    sys.exit(0)

def signal_handler(signum, frame):
    logging.info(f"Received signal {signum}")
    shutdown_server()

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    
    try:
        app.run(host='0.0.0.0', port=8080, debug=False)
    except KeyboardInterrupt:
        logging.info("Keyboard interrupt received")
        shutdown_server()