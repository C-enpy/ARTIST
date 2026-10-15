---
description: Isole un défaut de rendu avec une reproduction et des preuves CPU/GPU.
agent: gpu-engineer
subagent: true
---

Diagnostique $ARGUMENTS. Charge artist-gpu-debug. Cherche une reproduction bornée,
vérifie le contexte puis suis ressources, états et shaders jusqu'au premier écart.
Cette commande est diagnostique : ne modifie pas les sources. Rapporte la cause
prouvée ou la prochaine expérience discriminante, les captures réellement faites,
les préconditions manquantes et le correctif minimal proposé.
