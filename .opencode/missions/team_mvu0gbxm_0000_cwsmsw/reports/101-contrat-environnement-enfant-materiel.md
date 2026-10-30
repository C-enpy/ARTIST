# 101 — Corriger l'environnement au moment du lancement

Pour éviter qu'une qualification matérielle hérite du forçage logiciel parent,
le coordinateur retient la correction privée proposée par l'architecte : nettoyer
seulement la copie enfant des commandes déclarant explicitement le profil matériel.
Le parent, les commandes CPU et le profil logiciel restent inchangés.

Les six valeurs de transport doivent être explicites ; une commande tentant de
réintroduire les deux variables de forçage sera refusée avant lancement. Aucun
changement du format des commandes, aucune valeur vide artificielle, aucun choix
de driver d'après le nom du GPU. Ce nettoyage ne démontre pas la présence d'un
renderer matériel ; les contrôles de cible restent nécessaires.

Le contrat est persisté avant implémentation. Le complément QA doit encore être
reçu et commité avant admission d'un auteur unique sur le runner partagé et ses
tests. L'auteur Linux conserve son candidat stable ; il n'est pas autorisé à
modifier ce fichier commun. Les branches Windows restent indépendantes.

## Traçabilité

- Avis architecture `result_mvumaqrx_0000_w3jsme`, lecture seule sans exécution.
- Contrat : `.opencode/evidence/mission-uniform-pipeline/hardware-child-environment-v1.md`.
- Planification locale de cette petite correction indivisible : PM/LLM, complexité
  brute62/retenue70, risque65, incertitude moyenne ; plancher développement respecté,
  sans changement de grille ou modèle. Tests et correction gardent un seul propriétaire.
- Finding source : rapport100 ; GREEN Linux antérieur : rapport99. Aucun rerun.

Reproduction dynamique encore NON_EXECUTE. Aucun moteur, OpenGL, outil natif,
rendu, benchmark, installation ou cloud lancé.
