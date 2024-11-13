from concurrent.futures import ThreadPoolExecutor
import logging

class ReceiverManager:
    def __init__(self, engine_manager):
        self.engine_manager = engine_manager
        self.executor = ThreadPoolExecutor(max_workers=1)
        self.future = None

    def start(self):
        if self.future is None or self.future.done():
            eng = self.engine_manager.init_matlab_engine()
            self.future = self.executor.submit(lambda: eng.Receiver(nargout=0))
            self.future.add_done_callback(lambda f: logging.info("Receiver script completed"))
            return 'Started Receiver', 200
        else:
            return 'Receiver is already running', 400

    def stop(self):
        if self.future and not self.future.done():
            with open('SDR/stop_receiver.txt', 'w') as f:
                f.write('stop')
            logging.info("Stop signal sent to Receiver via stop_receiver.txt")
            return 'Stopped Receiver', 200
        else:
            return 'Receiver is not running', 400

    def control(self, action):
        if action == 'start':
            return self.start()
        elif action == 'stop':
            return self.stop()
        else:
            return 'Invalid action for receiver', 400

    def get_status(self):
        return "Running" if self.future and not self.future.done() else "Stopped"

    def shutdown(self):
        self.executor.shutdown(wait=False)