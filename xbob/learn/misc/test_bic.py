#!/usr/bin/env python
# vim: set fileencoding=utf-8 :
# Manuel Guenther <Manuel.Guenther@idiap.ch>
# Thu Jun 14 14:45:06 CEST 2012
#
# Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland

"""Test BIC trainer and machine
"""

import numpy
from . import BICMachine, BICTrainer

def equals(x, y, epsilon):
  return (abs(x - y) < epsilon).all()

class BICTrainerAndMachineTest(unittest.TestCase):
  """Performs various BIC trainer and machine tests."""

  def training_data(self):
    data = numpy.array([
      (10., 4., 6., 8., 2.),
      (8., 2., 4., 6., 0.),
      (12., 6., 8., 10., 4.),
      (11., 3., 7., 7., 3.),
      (9., 5., 5., 9., 1.)], dtype='float64')

    return data, -1. * data

  def eval_data(self, which):
    eval_data = numpy.ndarray((5,), dtype=numpy.float64)
    if which == 0:
      eval_data.fill(0.)
    elif which == 1:
      eval_data.fill(10.)

    return eval_data

  def test_IEC(self):
    # Tests the IEC training of the BICTrainer
    intra_data, extra_data = self.training_data()

    # train BIC machine
    machine = BICMachine()
    trainer = BICTrainer()

    # train machine with intrapersonal data only
    trainer.train(machine, intra_data, intra_data)
    # => every result should be zero
    self.assertAlmostEqual(machine(self.eval_data(0)), 0.)
    self.assertAlmostEqual(machine(self.eval_data(1)), 0.)

    # re-train the machine with intra- and extrapersonal data
    trainer.train(machine, intra_data, extra_data)
    # now, only the input vector 0 should give log-likelihood 0
    self.assertAlmostEqual(machine(self.eval_data(0)), 0.)
    # while a positive vector should give a positive result
    self.assertTrue(machine(self.eval_data(1)) > 0.)

  def test_BIC(self):
    # Tests the BIC training of the BICTrainer
    intra_data, extra_data = self.training_data()

    # train BIC machine
    trainer = BICTrainer(2,2)

    # The data are chosen such that the third eigenvalue is zero.
    # Hence, calculating rho (i.e., using the Distance From Feature Space) is impossible
    machine = BICMachine(True)
    def should_raise():
      trainer.train(machine, intra_data, intra_data)
    self.assertRaises(RuntimeError, should_raise)

    # So, now without rho...
    machine = BICMachine(False)

    # First, train the machine with intrapersonal data only
    trainer.train(machine, intra_data, intra_data)

    # => every result should be zero
    self.assertAlmostEqual(machine(self.eval_data(0)), 0.)
    self.assertAlmostEqual(machine(self.eval_data(1)), 0.)

    # re-train the machine with intra- and extrapersonal data
    trainer.train(machine, intra_data, extra_data)
    # now, only the input vector 0 should give log-likelihood 0
    self.assertAlmostEqual(machine(self.eval_data(0)), 0.)
    # while a positive vector should give a positive result
    self.assertTrue(machine(self.eval_data(1)) > 0.)
