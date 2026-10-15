---
description: Coordonne une revue ciblée du diff ARTIST et des preuves adaptées à son risque.
agent: orchestrator
subagent: false
---

Examine $ARGUMENTS ; si vide, le diff courant, fichiers non suivis pertinents inclus.
Définis le contrat et sélectionne seulement les revues indépendantes utiles :
correctness, shaders, performance ou architecture. Fournis aux reviewers les
mêmes chemins et état du diff. Ne modifie pas les sources pour cette commande.
Rends les findings prouvés avec chemins/lignes, les hypothèses séparées et les
preuves manquantes. Ne transforme pas l'absence de finding en validation GPU.
