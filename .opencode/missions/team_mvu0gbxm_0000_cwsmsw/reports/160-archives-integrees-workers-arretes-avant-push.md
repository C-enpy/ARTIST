# 160 — Archives intégrées et workers arrêtés avant publication

Objectif : publier tout le travail de cette mission sur main, fermer les sessions
et archiver la mission sans réintroduire les essais remplacés.

Les neuf commits uniques et six brouillons sont préservés dans les archives
inertes committées en8b684cb. Les patches ont été reconstruits et comparés aux
arbres originaux ; les snapshots et le bundle ont été vérifiés avant retrait.

Tous les workers encore idle ont reçu un shutdown natif réussi, sans force.
La divergence du checkout pool-29 a été réconciliée par un switch Git normal
vers la branche attendue, propre et ancêtre de main, après archive du checkpoint.

Les seules pathlists historiques de pool-6/13/15/19 ont ensuite été alignées sur
main et committées administrativement. Les quatre fichiers non suivis de pool-19
absents de main ont été retirés de cette copie seulement après double vérification
de leurs hashes et des snapshots versionnés. Leur contenu reste dans les archives.
Les fusions team_merge natives des quatre branches ont réussi sans delta du produit.
Les quatre autres branches propres (29/30/42/43) n'ont aucun changement à fusionner.

Après chaque résultat natif, le diff intégré a été contrôlé : aucun changement
de source de mission, index vide, et patch des20 profils étrangers identique.
Les empreintes des sources testées ont également été recoupées après la première
fusion, sans divergence. Aucune campagne de test relancée.

Deux erreurs d'outillage ont été corrigées sans contournement : un diagnostic Git
attendu non nul a arrêté PowerShell avant mutation, remplacé par ls-tree ; une
chaîne JavaScript de diagnostic mal échappée a été corrigée dans Context Mode.

Prochaines étapes autorisées : commit explicite de ce suivi, push normal de main,
comparaison SHA distant/local, puis cleanup natif sans force/purge/acknowledgement.
La clôture n'est pas encore annoncée : elle exige le reçu d'archivage natif.

Références : rapports158/159, closure-qa-plan.md, manifeste versionné sous
research/mission-uniform-pipeline-closure/ et inventaire brut sous
out/verification-synthetic/mission-close-inventory-2026-10-30T13-43-45-896Z/.
Les modifications étrangères (profils, trois autres rapports, .tmp) restent
locales et exclues des commits. Qualification Windows/Linux/GPU NON_EXECUTE.
