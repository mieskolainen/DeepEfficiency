# DeepEfficiency network training
# ------------------------------------------------------------------------
#
# Requires: Python 3.x
#         + Tensorflow (tested with Tensorflow version 1.8)
#         + Common Python libraries
#
# Run with: python3 deepnet.py <train> <input>
#           python3 deepnet.py <predict> <input> <trained model>
#
#
# Tensorboard visualization:
# python ../tensorflow/bin/tensorboard --logdir=./train_summary
#
# mikael.mieskolainen@cern.ch, 23/07/2018


import sys
import numpy as np
import random
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
import time

import csv

print(tf.__version__)


# ------------------------------------------------------------------------
# GLOBAL = CREATE NETWORK STRUCTURE

EPSILON = 1e-7

# Input dimensionality
NDIM = 6

# Hidden layer nodes
N_NODES_HL = [96,96,96,96]

# Output nodes
N_CLASS = 1


# Number of samples
TRAINING_SAMPLES   = 1e7
PREDICTION_SAMPLES = 1e6



# Too high may easily skew the optimization
LEARNINGRATE = 0.001


# Regularization hyperparameter
BETA = 1e-5


# Large number of epoch is needed for truly learning the distribution
# (check (eta,phi)_track distributions after efficiency inversion)
EPOCHS  = 10

# Experiment with this (32, 64 etc. seems to work ok)
BATCH_SIZE = 64


x = tf.placeholder('float')
y = tf.placeholder('float')


# ------------------------------------------------------------------------
# Construct layer definitions

SIGMA = 0.1;

hidden_layer = []
i = 0
while i < len(N_NODES_HL)-1:

    INPUTDIM  = N_NODES_HL[i]
    OUTPUTDIM = N_NODES_HL[i+1]

    # Input layer as special case
    if (i == 0):
        INPUTDIM = NDIM

    # Construct layer and initialize with random and zero for bias
    # Random gives symmetry breaking
    hidden_layer.append( {'weight':tf.Variable(SIGMA * tf.random_normal([INPUTDIM, OUTPUTDIM])),
                            'bias':tf.Variable(tf.zeros([OUTPUTDIM]))} )

    # "He et al." style initialization would be: tf.sqrt(2/N_NODES_HL[i-1])
    i += 1

# Construct output layer and initialize with random and zero for bias
output_layer   = {'weight':tf.Variable(SIGMA * tf.random_normal([N_NODES_HL[len(N_NODES_HL)-1], N_CLASS])),
                    'bias':tf.Variable(tf.zeros([N_CLASS])),}


# Read in .csv data
def read_in_data(filename='', maxcount=1e15, readmode='GEN'):

    features = []
    labels   = []

    filename = './data/' + filename + ".csv"
    print("Reading input data from: %s" % (filename))

    k = 0
    with open(filename) as csvfile:
        reader = csv.DictReader(csvfile, delimiter=',', 
            fieldnames = ("px1_gen","py1_gen","pz1_gen","px2_gen","py2_gen","pz2_gen",
                          "px1_rec","py1_rec","pz1_rec","px2_rec","py2_rec","pz2_rec",
                          "pidCode1", "pidCode2", "reco"))
        for row in reader:

            # NOTE HERE, it is very important to have [ ] encapsulating done right below,
            # otherwise we get TensorFlow dimension errors such as
            # "logits and labels must be broadcastable: logits_size=[128,20] labels_size=[1,128]"


            # Note:
            # - When one assumes that the efficiency manifold varies
            #   smoothly locally in terms of reconstruction smearing/folding effects,
            #   one can safely use the generator level vectors as the training reference.
            # - This strategy works fine for precise (track) measurements.
            #   For highly smeared (e.g. calo) measurements, one should fuse this with unfolding.
            #   A multidimensional mapping for that to be studied.

            # Generator level
            if (readmode == 'GEN'):
                features.append([row['px1_gen'], row['py1_gen'], row['pz1_gen'],
                                 row['px2_gen'], row['py2_gen'], row['pz2_gen']])

            # Reconstruction level
            if (readmode == 'REC'):
                features.append([row['px1_rec'], row['py1_rec'], row['pz1_rec'],
                                 row['px2_rec'], row['py2_rec'], row['pz2_rec']])


            labels.append([float(row['reco'])])
            k += 1
            if (k >= maxcount):
                break

    return features, labels


# ------------------------------------------------------------------------
# Network model definition
def neural_network_model(data):

    # hidden layer 0: func(data * W) + b
    # hidden layer 1: func(hidden_layer_0 * W) + b
    # ...
    # hidden layer N: func(hidden_layer_N-1 * W) + b

    lx = data
    i  = 0
    while i < len(hidden_layer):
        lx = tf.add(tf.matmul(lx, hidden_layer[i]['weight']), hidden_layer[i]['bias'])
        #lx = tf.sigmoid(lx)
        #lx = tf.nn.elu(lx)
        lx = tf.tanh(lx)
        i += 1

    # output: func(hidden_layer_N * W) + b
    output = tf.add(tf.matmul(lx, output_layer['weight']), output_layer['bias'])
    output = tf.sigmoid(output)

    return output


# ------------------------------------------------------------------------
# KL-divergence
def KL_costfunc(prediction, reference):
    reference  = tf.clip_by_value(reference,  EPSILON, 1-EPSILON)
    prediction = tf.clip_by_value(prediction, EPSILON, 1-EPSILON)
    cost = tf.reduce_sum(reference * tf.log(reference / prediction))
    return cost

# Poisson loss
def poisson_costfunc(prediction, reference):
    cost = tf.reduce_sum(prediction - reference * tf.log(prediction + EPSILON))
    return cost

# L2-cost function ~ Gaussian max likelihood
def L2_costfunc(prediction, reference):

    cost = tf.reduce_sum(tf.square(prediction - reference))
    return cost

# Cross-Entropy cost
def CEX_costfunc(prediction, reference):

    prediction = tf.clip_by_value(prediction, EPSILON, 1-EPSILON)
    cost = -tf.reduce_sum(reference * tf.log(prediction) + (1-reference) * tf.log(1-prediction))
    return cost


# ------------------------------------------------------------------------
# Count number of free parameters in the network
def number_of_parameters():

    N_parameters = 0

    for variable in tf.trainable_variables():
        # shape is an array of tf.Dimension
        shape = variable.get_shape()
        print(shape)
        print(len(shape))
        param = 1
        for dim in shape:
            print(dim)
            param *= dim.value
        print(param)
        N_parameters += param
    
    print("Total number of network parameters: %d \n" % N_parameters)


# ------------------------------------------------------------------------
# Train the network
def train_neural_network(train_x, train_y, TRAININGFILE):

    if (len(train_x) != len(train_y)):
        print("train_neural_network:: Error: train_x length != train_y length")
        return
    print("Training with %d vectors" % (len(train_x)))
    print("- BATCH_SIZE = %d" % (BATCH_SIZE))
    print("- LEARNINGRATE = %0.5f" % (LEARNINGRATE))
    print("- BETA = %0.3E" % (BETA))


    # The model definition
    prediction = neural_network_model(x)

    # *** 1. Fidelity ***
    #fidelity = tf.losses.huber_loss(predictions=prediction, labels=y, delta=2.5)
    #fidelity = tf.losses.mean_squared_error(predictions=prediction, labels=y)
    #fidelity = L2_costfunc(prediction, y)
    fidelity = CEX_costfunc(prediction, y)

    # *** 2. Regularization applied to weight terms ***
    regularization = 0;
    i  = 0
    while i < len(hidden_layer):
        regularization += tf.nn.l2_loss(hidden_layer[i]['weight'])
        #regularization += tf.norm(hidden_layer[i]['weight'], ord=1)
        i += 1
    regularization += tf.nn.l2_loss(output_layer['weight'])
    #regularization += tf.norm(output_layer['weight'], ord=1)


    # **** Total cost ****
    cost = tf.reduce_mean(fidelity + BETA*regularization)


    # Optimization algorithm
    optimizer = tf.train.AdamOptimizer(LEARNINGRATE).minimize(cost)
    #optimizer = tf.train.GradientDescentOptimizer(LEARNINGRATE).minimize(cost)

    # Count How many free parameters
    number_of_parameters()

    # Start tensorflow session
    with tf.Session() as sess:

        #summary_writer = tf.summary.FileWriter('train_summary', sess.graph)

        # Variable initialization
        sess.run(tf.global_variables_initializer())

        # Add ops to save and restore all the variables.
        saver = tf.train.Saver()

        # Loop over the whole training sample by number of epoch count
        for epoch in range(EPOCHS):

            epoch_loss = 0
            i = 0

            # Training data splitted in training batches
            t0 = time.time()
            while i < len(train_x):
                start = i
                end   = i + BATCH_SIZE
                batch_x = np.array(train_x[start:end])
                batch_y = np.array(train_y[start:end])

                #print(batch_x)

                # Run optimization
                _, c = sess.run([optimizer, cost], feed_dict={x: batch_x, y: batch_y})
                epoch_loss += c
                i += BATCH_SIZE
                last_cost = c

            remain = float(EPOCHS-(epoch+1)) * float(time.time()-t0);
            print('Epoch %0.3d' % (epoch+1), '/ %d' % EPOCHS, ': Cost = %0.5f'
                    % last_cost, 'in %0.2f sec' % (time.time()-t0), 'Remaining %0.2f sec' % (remain))

        # Quick accuracy test (not so useful here)
        #cor_gent  = tf.equal(tf.argmax(prediction, 1), tf.argmax(y, 1))
        #accuracy = tf.reduce_mean(tf.cast(cor_gent, 'float'))
        #print('Accuracy:', accuracy.eval({x:train_x, y:train_y}))

        #output_weight = sess.run(output_layer['weight'])
        #output_bias   = sess.run(output_layer['bias'])

        # Save the variables (model) to disk
        save_path = saver.save(sess, "./modelsave/DEEPNET_" + TRAININGFILE + ".ckpt")
        print("Model saved in path: %s" % save_path)


# ------------------------------------------------------------------------
# Run predictions using the network
def predict_neural_network(input_x, outputfile, inputfile):

    # Write out predictions here
    outputfile = './output/' + outputfile + '.out'
    myfile = open(outputfile, 'w')

    start = time.time()

    with tf.Session() as sess:

        # Initialization
        #tf.reset_default_graph()
        saver = tf.train.Saver()

        # The model definition
        prediction = neural_network_model(x)

        # Restore variables from disk.
        networkfile = "./modelsave/DEEPNET_" + inputfile + ".ckpt"
        saver.restore(sess, networkfile)
        print("Using network model: %s" % networkfile)
        print("Model loaded from file.")

        # Evaluate model predictions
        output = prediction.eval(feed_dict = {x: input_x})

        # print predictions using our model
        #for i in enumerate(input_x):
        print("Saving prediction output to: ", outputfile)
        for i in range(0, len(input_x)):

            prob1 = output[i][0]
            myfile.write("%0.6f \n" % prob1)

    print('Prediction done for %d vectors in %0.3f sec' % (len(input_x), time.time() - start))
    myfile.close();


# ------------------------------------------------------------------------
# Main function
def main(argv):

    # 1. TRAIN THE NETWORK
    if (argv[1] == 'train'):
        TRAININGFILE = argv[2]
        print("TRAINING mode:: Train input: %s" % TRAININGFILE)
        train_x, train_y = read_in_data(filename=TRAININGFILE, maxcount=TRAINING_SAMPLES, readmode='GEN')
        train_neural_network(train_x, train_y, TRAININGFILE)

    # 2. USE THE NETWORK FOR PREDICTIONS
    elif (argv[1] == 'predict'):
        PREDICTFILE  = argv[2]
        TRAININGFILE = argv[3]
        print("PREDICTION mode:: Prediction input: %s" % PREDICTFILE)
        test_x, test_y   = read_in_data(filename=PREDICTFILE, maxcount=PREDICTION_SAMPLES, readmode='REC')
        predict_neural_network(test_x, PREDICTFILE, TRAININGFILE)

    else:
        print("DeepEfficiency estimator")
        print("  Usage: ./deepnet <mode>")
        print("  <mode> = train or predict")

# Call main
if __name__ == "__main__":
   main(sys.argv)


"""
plt.title('output layer weights')
plt.imshow(output_weight.reshape([2, 20]), cmap=plt.get_cmap('seismic'))
frame1 = plt.gca()
frame1.axes.get_xaxis().set_visible(False)
frame1.axes.get_yaxis().set_visible(False)
plt.show()
output_bias
"""
