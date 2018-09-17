# DeepEfficiency predictions
#
# mikael.mieskolainen@cern.ch, 26/07/2018

# pi+pi-
MODEL=tree2track_kPipm
python3 deepnet.py predict tree2track_kPipmExp $MODEL
python3 deepnet.py predict tree2track_kPipmOrexp $MODEL
python3 deepnet.py predict tree2track_kPipmPower $MODEL
python3 deepnet.py predict tree2track_kCohRhoToPi $MODEL

# K+K-
MODEL=tree2track_kKpkm
python3 deepnet.py predict tree2track_kKpkmExp $MODEL
python3 deepnet.py predict tree2track_kKpkmOrexp $MODEL
python3 deepnet.py predict tree2track_kKpkmPower $MODEL
