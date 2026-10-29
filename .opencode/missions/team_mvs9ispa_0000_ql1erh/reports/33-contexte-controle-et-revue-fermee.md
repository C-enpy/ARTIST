# Contexte réutilisable — contrôles terminés

Le mandat de publication hors `.tmp/` est prêt à être finalisé sur main. Les
profils/règles, validateurs, notes, rapports, politiques historiques et exports
publics sont inclus ; aucun moteur, shader, build ou workflow CI modifié.

Deux points ont été raccordés avant publication : reconnaissance du contrat v2
déjà commun aux trois QA et test protégeant l'interdiction de transformer le
planner en runner. RED causaux conservés, **634/634 tests** finaux,0échec/skip ;
syntaxe/schéma/hors ligne PASS. Revue indépendante et ré-audit reçus, finding fermé.
Ce résultat reste statique/hors ligne, pas un PASS d'enforcement natif ou de GPU.

La configuration GrepAI active est retirée de l'index seulement et conservée sur
disque, identique par empreinte ; l'exemple public remplace la clé par un marqueur.
Les deux fichiers `.tmp` sont exclus et préservés. Les blobs candidats ont été
contrôlés sans imprimer la clé ; aucune correspondance trouvée. La valeur ancienne
dans l'historique était un placeholder, pas la clé active.

Les11exports publics conservent leurs SHA256,385315octets. Ils n'incluent pas de
raisonnement privé selon leur manifeste et ne constituent pas des preuves
d'exécution nouvelles. Les espaces finaux de ces transcriptions et du patch
historique restent préservés pour ne pas casser leurs identités ; le diffcheck
hors archives est propre. Un fichier bénéficie de la normalisation CRLF standard
de Git, sans mutation du fichier local.

Preuves : `.opencode/evidence/context-publication/{qa-plan,qa-review}.md` et
`out/logs/context-publication/`. Le lead termine le commit, le fetch/push sans
force et le contrôle des références ; aucune nouvelle approbation de modèle,
grant ou démarrage de mission n'est induit par ces fichiers historiques.
