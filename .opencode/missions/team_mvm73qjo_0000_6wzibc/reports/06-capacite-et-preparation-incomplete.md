# 06 — Capacité provider indisponible ; préparation incomplète

Date : 2026-10-24. Étape : enquêtes préalables, admissions supplémentaires suspendues. Référence : [rapport 05](05-demarrage-et-limite-transport.md).

## Changements et résultats reçus

La récupération non filtrée `team_results` a livré des messages que les dernières récupérations filtrées ne montraient pas. Le décalage de notification ne doit pas être assimilé à une absence de résultat définitive.

1. **ASan/protocole : BLOCKED infrastructure.** Message système : `Cursor Run failed after 3 attempts`, erreur `capacity_limit`. Aucun résultat accepté, aucune tâche complétée. Le runtime avait sélectionné `cursor/claude-opus-5-5` Max dans le catalogue approuvé. Les trois tentatives provider sont épuisées ; le lead ne lance ni répétition plus coûteuse ni substitution de modèle par spawn/message. Ce n'est pas RED ni un échec du moteur.
2. **QA/pool-2 : préparation partielle seulement.** Résultat `failed` reçu : arrêt demandé, aucun H_Q livré ni tâche clôturée, aucune écriture/compilation/test/commit déclaré. Les observations statiques détaillées portent sur la fiche et les tests/Reader/helpers. Le résultat ne comprend pas le registre complet, le budget, les I×P B5/B6 ni un HEAD courant. Il ne remplace donc pas le plan QA préalable et ne débloque aucun auteur. La provenance de ce membre appartient à la session antérieure : ne pas le présenter comme une nouvelle admission du lead dans cette étape.
3. **F8 :** la reprise bornée avec sources QMD fournies par le lead a été envoyée ; aucun nouveau H_A n'a été reçu dans cette récupération. Le message du lead apparaît dans le résultat de collecte, ce qui ne prouve pas que l'analyse a été achevée ou même reprise.

## Observations statiques à transmettre, sans PASS

- B1 : la note reçue mentionne seed `20261017` donnant un seed effectif `61219`, et seed 7 ; enregistrer impérativement l'ordre/seed effectif dans les futurs essais. Préexistence n'est pas cause démontrée.
- B3 : la note distingue déjà tests de seam et observation close-avant-publication des pannes réelles non exercées. Préserver cette distinction pour H_A.
- B4 : garde ProgramUnbinder existante, ASSERT potentiellement avant débind manuel ; helper utilisant une sous-chaîne. Négatifs proposés : message voisin préfixé/suffixé, type incorrect, absence d'exception. Allowlist/ViewTests encore non instruits.
- B5 : 20:35 et 20:38 ont des comptages et cas divergents dans les notes historiques ; ni pin vérifié ni reproduction actuelle. C2 reste ouvert.
- B6 : les tests MSVC existants n'exercent pas le corps conditionnel libstdc++ ; GCC natif non validé.

Ces points sont des témoignages statiques reçus, pas une vérification indépendante du lead ni une preuve exécutée. Ils doivent être recoupés à HEAD avant d'être utilisés comme constat courant.

## État des barrières

H_Q absent ; H_A absent ; sélection des 17 GL absente. Aucun auteur autorisé à écrire, aucun livrable intégré, aucune campagne/test exécuté par le lead. Le protocole ASan est bloqué par capacité, pas par un besoin d'étendre la plateforme ou le harness. Aucune modification de permissions ou du catalogue effectuée après l'approbation.

## Options, compromis et recommandation

- **A — Réviser cette mission sans Cursor (recommandé).** Désactiver uniquement ses entrées dans le brouillon, conserver les métriques et autres providers, présenter `/ensemble-mission config`, puis laisser l'utilisateur approuver la révision avec `/ensemble-mission start`. Cette restriction explicite rend le catalogue compatible avec la préférence OpenAI/OpenCode Go/Z AI et évite le provider actuellement indisponible. Elle ne garantit pas qu'un autre provider a de la capacité ; le runtime reste propriétaire du choix exact. Aucun changement des admissions passées : la tâche échouée doit être rendue réadmissible selon le contrat natif, sans réécriture de son identité/session/modèle déjà admis.
- **B — Attendre la capacité Cursor.** Aucun changement de politique ; coût/délai inconnus. Nouvelle tentative seulement sur demande explicite et via la reprise native permise, pas boucle de retry autonome.

Décision demandée : choisir A ou B. Il s'agit d'un choix de politique/reprise après épuisement des tentatives supervisées, pas d'une approbation de correctif moteur ni d'un élargissement QA. Les autres prérequis et limites restent inchangés.
