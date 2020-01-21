# DeepEfficiency
https://arxiv.org/abs/1809.06101

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

<img width="500px" src="figs/deepnet.png">

**Requirements: Python3 & Tensorflow 1.8+ & ROOT libraries**
</br>
</br>
*This is a research level proof-of-principle code. Depending on the physics application, additional algorithms, estimators and regularization techniques may be needed.*
</br>

### Get ascii (.csv) out from ROOT trees
```
root printascii.c+ -b -q
```

### Train DeepEfficiency networks
```
train.sh
```

### Obtain efficiency inversion estimates
```
predict.sh
```

### Plot differential distributions
```
make && ./deeplot
```
</br>

## Citation

If you find this work useful, you may cite the paper:

```
@article{mieskolainen2018deepefficiency,
    title={DeepEfficiency - optimal efficiency inversion in higher dimensions at the LHC},
    author={Mikael Mieskolainen},
    year={2018},
    journal={arXiv:1809.06101},
    eprint={1809.06101},
    archivePrefix={arXiv},
    primaryClass={physics.data-an}
}
```
