from msl.loadlib import Client64

class IngaasCamConfCl(Client64):
    """Call functions in 'snakecamlinkcontrol.dll' via the 'IngaasCamConfSrv' wrapper."""

    exposure = 10 # ms
    gain = 0 # 0 or 1
    ifaceOK = False
    camOK = False
    closed = False

    def __init__(self, hgain = 0):
        # Specify the name of the Python module to execute on the 32-bit server (i.e., 'my_server')
        super(IngaasCamConfCl, self).__init__(module32='IngaasCamConfSrv')
        self.ifaceOK = self.request32('init_iface', hgain)
        self.closed = False

    def __del__(self):
        if not self.closed:
            self.Close()
    
    def Close(self):
        self.request32('free_psl')
        self.shutdown_server32()
        self.closed = True
        
    def SetExposure(self, expms):
        if expms < 1e-3:
            expms = 1e-3
        self.request32('SetExposure', expms)
        self.exposure = expms
        
    def SetGain(self, hgain):
        self.request32('SetGain', hgain)
        self.gain = hgain
        
    def GetExposure(self):
        return self.exposure
    
    def GetGain(self):
        return self.gain
