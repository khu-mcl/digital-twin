from concurrent.futures import ThreadPoolExecutor
import logging

class TransmitterManager:
    def __init__(self, engine_manager):
        self.engine_manager = engine_manager
        self.executor = ThreadPoolExecutor(max_workers=1)
        self.future = None
        self.status = "Stopped"

    def start(self):
        if self.future is None:
            eng = self.engine_manager.init_matlab_engine()
            self.future = self.executor.submit(lambda: eng.Transmitter(nargout=0))
            self.future.add_done_callback(lambda f: logging.info("Transmitter script completed"))
            self.status = "Running"
            return 'Started Transmitter', 200
        else:
            return 'Transmitter is already running', 400

    def stop(self):
        if self.future:
            eng = self.engine_manager.init_matlab_engine()
            eng.eval("release(plutoTx)", nargout=0)
            self.status = "Stopped"
            return 'Stopped Transmitter', 200
        else:
            return 'Transmitter is not running', 400

    def control(self, action):
        if action == 'start':
            return self.start()
        elif action == 'stop':
            return self.stop()
        else:
            return 'Invalid action for transmitter', 400

    def get_status(self):
        return self.status

    def shutdown(self):
        self.executor.shutdown(wait=False)