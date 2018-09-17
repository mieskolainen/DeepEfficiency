# DeepEfficiency training
#
# mikael.mieskolainen@cern.ch, 26/07/2018

# pi+pi-
MODEL=tree2track_kPipm
python3 deepnet.py train $MODEL

# K+K-
MODEL=tree2track_kKpkm
python3 deepnet.py train $MODEL
