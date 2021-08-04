#!/usr/bin/env groovy

@Library('etn-ipm2-jenkins@feature/add-feature-image-for-coverity') _

import params.CmakePipelineParams
CmakePipelineParams parameters = new CmakePipelineParams()
parameters.debugBuildRunCoverage = true

etn_ipm2_build_and_tests_pipeline_cmake(parameters)

