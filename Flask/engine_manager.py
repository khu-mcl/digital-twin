import matlab.engine
import logging

class EngineManager:
    def __init__(self):
        self.eng = None

    def init_matlab_engine(self):
        if self.eng is None or not self.eng._check_matlab():
            logging.info("Initializing MATLAB engine...")
            self.eng = matlab.engine.start_matlab('-nodisplay')
            self.eng.cd(r'C:/Users/hmk61/Desktop/MCLDigitalTwin/SDR')
        return self.eng

    def get_status(self):
        return "Running" if self.eng and self.eng._check_matlab() else "Stopped"

    def shutdown(self):
        if self.eng:
            logging.info("Closing MATLAB engine...")
            self.eng.quit()