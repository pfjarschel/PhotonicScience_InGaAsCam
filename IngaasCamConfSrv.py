import ctypes
from msl.loadlib import Server32

class IngaasCamConfSrv(Server32):
    """Wrapper around a 32-bit C++ library 'snakecamlinkcontrol.dll'."""

    exposure = 10 # ms
    gain = 0 # 0 or 1
    cam = None
    ifaceOK = False
    camOK = False

    def __init__(self, host, port, **kwargs):
        # Load the 'my_lib' shared-library file using ctypes.CDLL
        super(IngaasCamConfSrv, self).__init__("./snakecamlinkcontrol.dll", 'cdll', host, port)

    # The Server32 class has a 'lib' property that is a reference to the ctypes.CDLL object
    
    def __del__(self, **kwargs):
        pass

    def init_iface(self, hgain=0):
        campath = ""
        if hgain == 1:
            campath = "ingaas_files/Snake_High_Gain/PSL_camera_files"
        else:
            campath = "ingaas_files/Snake_Mid_Gain/PSL_camera_files" 
        
        ptype = ctypes.POINTER(ctypes.c_char_p)
        caminit = self.lib.PSL_VHR_Init
        caminit.argtypes = [ptype]        
        campathb = campath.encode(encoding="utf-8")
        campathca = ctypes.create_string_buffer(campathb)
        self.ifaceOK = not caminit(ctypes.cast(ctypes.addressof(campathca), ptype))

        return self.ifaceOK

    def free_psl(self):
        if self.ifaceOK:
            self.lib.PSL_VHR_Free()
        
    def SetExposure(self, expms):
        if self.ifaceOK: 
            if expms < 1e-3:
                expms = 1e-3
            self.lib.PSL_VHR_WriteExposure(1000*expms)
            self.exposure = expms
        
    def SetGain(self, hgain):
        if self.ifaceOK:            
            if hgain == 1:
                self.lib.PSL_VHR_enable_high_gain_mode(True)
            else:
                self.lib.PSL_VHR_enable_high_gain_mode(False)
            self.gain = hgain