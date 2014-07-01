from modules.common import *
import numpy as np
import os

halostruct = np.dtype([('id',np.int64),
                       ('pos',np.float32,(6,)),
                       ('corevel',np.float32,(3,)),
                       ('bulkvel',np.float32,(3,)),
                       ('m',np.float32),
                       ('r',np.float32),
                       ('child_r',np.float32),
                       ('vmax_r',np.float32),
                       ('mgrav',np.float32),
                       ('vmax',np.float32),
                       ('rvmax',np.float32),
                       ('rs',np.float32),
                       ('klypin_rs',np.float32),
                       ('vrms',np.float32),
                       ('J',np.float32,(3,)),
                       ('energy',np.float32),
                       ('spin',np.float32),
                       ('alt_m',np.float32,(4,)),
                       ('Xoff',np.float32),
                       ('Voff',np.float32),
                       ('b_to_a',np.float32),
                       ('c_to_a',np.float32),
                       ('A',np.float32,(3,)),
                       ('b_to_a2',np.float32),
                       ('c_to_a2',np.float32),
                       ('A2',np.float32,(3,)),
                       ('bullock_spin',np.float32),
                       ('kin_to_pot',np.float32),
                       ('m_pe_b',np.float32),
                       ('m_pe_d',np.float32),
                       ('dummy1',np.float32),              ## ALIGNMENT
                       ('num_p',np.int64),
                       ('num_child_particles',np.int64),
                       ('p_start',np.int64),
                       ('desc',np.int64),
                       ('flags',np.int64),
                       ('n_core',np.int64),
                       ('dummy2',np.float32),              ## ALIGNMENT
                       ('min_pos_err',np.float32),
                       ('min_vel_err',np.float32),
                       ('min_bulkvel_err',np.float32)
                   ])


class RockstarFile(object):
    
    def __init__(self,binfile,data):
        self.binfile = binfile
        self.header()

        self.halos()
        if data == 'particles':
            self.particles()
        self.f.close()
        
    def header(self):
        f = open(self.binfile,'rb')
        f.seek(8*3 + 4*10,1)
        self.num_halos     = np.fromfile(f,dtype=np.int64,count=1)[0]
        self.num_particles = np.fromfile(f,dtype=np.int64,count=1)[0]
        bytes_left = 256 - f.tell()
        f.seek(bytes_left,1)
        self.f = f
    
    def halos(self):
        self.halodata = np.fromfile(self.f,dtype=halostruct,count=self.num_halos)
        
    def particles(self):
        self.particle_IDs     = np.zeros(self.num_particles,dtype=np.int64)
        self.particle_IDs.fill(-1)
        self.particle_haloIDs = np.zeros(self.num_particles,dtype=np.int64)
        self.particle_haloIDs.fill(-1)

        nparts = 0
        for i in range(0,self.num_halos):
            hid   = self.halodata[i]['id']
            num_p = self.halodata[i]['num_p']
            pids  = np.fromfile(self.f,dtype=np.int64,count=num_p)
            
            self.particle_IDs[nparts:nparts+num_p]     = pids
            self.particle_haloIDs[nparts:nparts+num_p] = hid
            nparts += num_p


def compileReturnArray(RS,data):
    """compile data from RS binary and return requested value"""
    arr = []
    singleval = False

    ## return particle ID data
    if data == 'particles':
        npart = 0
        for i in range(0,len(RS)):
            npart += len(RS[i].particle_IDs)

        arr   = np.zeros((npart,2),dtype=np.int64)
        npart = 0
        for i in range(0,len(RS)):
            n = len(RS[i].particle_IDs)
            arr[npart:npart+n,0] = RS[i].particle_IDs
            arr[npart:npart+n,1] = RS[i].particle_haloIDs
            npart += n

        return arr

    ## return halo struct data
    if data in halostruct.names:
        singleval = True
        print '%s found in halodata' % data

    nhalos = 0
    for i in range(0,len(RS)):
        nhalos += RS[i].num_halos
        if singleval:
            arr.extend(RS[i].halodata[data])
        else:
            arr.extend(RS[i].halodata)

    #print nhalos,len(arr)
    return np.asarray(arr)
    


def readrockstar(binfile,data,**kwargs):
    """read rockstar binary file"""
    RS_DATA = []
    for j in range(0,5000):
        b = '%s.%d.bin' % (binfile,j)
        if os.path.isfile(b):
            #print 'reading %s' % b
            RS_DATA.append(RockstarFile(b,data))
        else:
            break

    arr = compileReturnArray(RS_DATA,data)
    return arr