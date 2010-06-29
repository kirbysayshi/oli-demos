#!/bin/bash
java -jar ../compiler-latest/compiler.jar --compilation_level=ADVANCED_OPTIMIZATIONS \
--js=mjs/mjs.js \
--js=FPSMonitor.js \
--js=Inheritance.js \
--js=VectorUtil.js \
--js=CParticle.js \
--js=CBody.js \
--js=CLinConstraint.js \
--js=CRigidBody.js \
--js=CBox.js \
--js=CSoftBody.js \
--js=CChain.js \
--js=CVertex.js \
--js=CMesh.js \
--js=CMeshBlob.js \
--js=CBodyList.js \
--js=CWorld.js \
\
--js_output_file=OLIENGINE.js