# 114 — Deux branches restantes, puis réception intégrée

Les adaptateurs sont intégrés, mais Windows prepare/build est encore volontairement
fermé. Le PM a actualisé le graphe restant en lecture seule ; le coordinateur le
retient avec le complément QA déjà demandé avant toute écriture auteur.

## Branches indépendantes après plan QA commité

1. **Helper Windows fonctionnel** : prepare/build seulement, garde d'entrée avant
   effets, payload validé, environnement outil et restauration, code enfant conservé,
   aucun fallback legacy/run. Ownership helper et tests Windows ; régions pinées
   inchangées par défaut. Tests RED par fragments extraits explicitement et feuilles
   factices, jamais helper complet. Ajouter le contrôle effectif WindowsAdapter→factory
   manquant. PM/LLM brut/retenu78, risque68, incertitude moyenne.
2. **Raccordement CMake et workflow manuel** : option workloads OFF par défaut,
   inclusion conditionnelle indépendante de BUILD_TESTING après ArtistLib, aucun
   auto-run CTest, campagnes existantes préservées. Ownership parents CMake, nouveau
   workflow et tests statiques dédiés ; helpers/workloads en lecture. RED statique
   puis GREEN par parsers/doubles, jamais vrai configure/build. PM/LLM brut53,
   retenu70, risque44, incertitude moyenne.

Ces branches n'écrivent aucun fichier commun ; elles peuvent avancer en parallèle.
Le coordinateur reste runner et sérialise les exécutions qui partagent un état.
Le helper reçoit une revue indépendante après GREEN (64/62) ; le raccordement
CMake/workflow aura les contrôles de configuration adaptés, sans campagne réelle.

## Consommateurs gardés dans le plan, non publiés

Après intégration et vérification des deux branches, le generator QA examine les
seuls gaps démontrés et propose des tests sans production ni exécution (brut55,
retenu70 si patch de test, risque47). En parallèle, la photographie documentaire
finale peut être écrite sur les entrées intégrées (35/30). Les propositions retenues
reviennent aux propriétaires et sont intégrées avant l'audit final fresh indépendant
(58/53), hors board d'implémentation. Aucun stage ne devient un nouvel accord
utilisateur ; aucune fin d'auteur ne libère implicitement ses consommateurs.

Les deux scores sont validés comme PM/LLM, non des probabilités. Le plancher70
est conservé pour tout code/test/script/config exécutable ; ni modèle explicite
ni changement de grille. Les sous-tâches RED/GREEN restent sous un propriétaire
unique, pas réparties artificiellement entre agents.

## Bornes et provenance

- Consultation PM autonome `ses_e743eeae9ffevTZLCaGDtPRXEu`, lecture seule, sans
  édition, test ou délégation. HEAD fourni `a7aacb9`, non vérifié par Git par le PM.
- Réception adaptateurs : rapports112/113 ; état natif de fermeture Windows lu
  dans `scripts/build-windows.ps1:517–522`, dispatch prototype524–576.
- Une suggestion de découverte globale Windows du PM est explicitement écartée :
  sélection nominative relue uniquement, test06 jonction NON_EXECUTE.
- Les anciennes tâches réadmises ne doivent pas créer un second écrivain. Un
  message d'inactivité a été envoyé à la reprise pool-29 ; aucun cycle de refus
  natif, arrêt/réadmission ou fausse complétion n'est employé pour masquer le suivi.

Aucun helper complet, outil natif, moteur, GPU, rendu, benchmark, installation
ou cloud exécuté par cette planification. Le plan QA des deux branches est attendu.
