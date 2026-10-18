# Checklist documentaire — ingestion Attribute-GL (périmètre réduit)

2026-10-18. Version proportionnée au périmètre réduit par l'utilisateur : une
note de session ; garde-fous au stade proposition, aucune règle opérationnelle
modifiée. Auteur/runner : lead. Aucun nouveau script, build ou test moteur.

Base : plan P0 reçu en lecture seule du qa-planner pool-1, candidat natif
`result_mve4hyfp_000m_9ya0ma`, message `msg_15023383a001VqdXWgWmyMIXg0`.
Scores P0 PM/LLM C54/R76 conservés ; I×P ne modifie pas le routage.
L'ancienne séquence P2–P9 n'est plus une autorisation de lancer des workers.

## Contrôles avant publication

- Confidentialité E1 : seulement le registre dérivé assaini ; jamais raw,
  raisonnement interne, prompts système, credentials ou état provider dans docs.
- Couverture E2/I1/I2 :13 sessions/850 entrées, borne inclusive et exclusion de
  l'ingestion ; ne pas confondre événements/messages ni promettre un verbatim.
- Attribution E3/E4 : chaque conclusion a sa source ; rectifications conservées,
  ancien résultat ≠ nouveau run, observation driver ≠ obligation moteur.
- Garde-fous E5 : propositions seulement, reliées aux règles canoniques existantes,
  sans duplication/adoption automatique ni promotion d'ADR.
- Ownership E6/E9 : note nouvelle, manifeste propre et backlink dans note propre
  seulement ; préserver tous les fichiers dirty externes, index/log inclus.
  Plan/checklist commité avant rédaction ; pas de worktree auteur à créer.
- Métadonnées E7 : frontmatter, sources, summary, liens résolus et entrée de
  manifeste hashée ; aucun lien du vault vers les exports privés.
- Retrouvabilité E8 : un indexeur, artist-wiki seulement, query avec intent + get
  après contrôle du diff ; aucun succès déduit de la simple présence du fichier.

## Risques du plan P0, maintenus comme rappel

Confidentialité I5×P5=25 : déclencheur copie brute ; impact Git/vault/QMD ; détection
lecture complète et contrôle de motifs sensibles ; mitigation dérivé assaini ;
gap audit externe de tout le corpus brut, non revendiqué.
Couverture I4×P5=20 : borne/snapshot erroné ; impact chronologie ; détection
comptages et hashes ; mitigation borne explicite ; gap contenu pré-compaction.
Fausse autorité I4×P4=16 : conclusion rectifiée présentée comme règle ; impact
futurs contrats ; détection sources/contre-exemples ; mitigation propositions et
statuts explicites ; gap clôture historique globale, hors ingestion.

## Sortie et limites

Setup déjà acquis ; capacité résiduelle :75% rédaction/contrôles,25% buffer de
correction, pas d'estimation d'heures. Pas de nouvel examen moteur ni d'enquête
de conformité driver. Preuves absentes restent NOT_RUN/INCONCLUSIVE.
Le lead conserve le registre preuve↔obligation dans le compte rendu de validation.
Des propositions ambiguës sont différées, jamais adoptées pour terminer le lot.
