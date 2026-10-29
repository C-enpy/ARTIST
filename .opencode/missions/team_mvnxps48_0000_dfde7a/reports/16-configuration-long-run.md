# 16 — Correction de configuration : long_run, classification manuelle conservée

2026-10-25. Demande native config de l'utilisateur, **pas approbation d'exécution**.

## Diff appliqué au seul brouillon de mission

Ajout du bloc transitoire de configuration :

```json
"classification": {
  "mode": "off",
  "missionMode": "long_run"
}
```

Dans ce contrat natif, mode off conserve la méthode manuelle sans classifier ; ce n'est pas un changement des scores ou du modèle. missionMode long_run permet aux travaux indépendants de continuer malgré un blocage local, avec arrêt d'admission sur échec critique. Le prochain config natif applique ce bloc puis le retire du JSON de politique ; ne pas déplacer missionMode au sommet de la politique V2 ni dans verification.

Inchangés : version2, providers openai/opencode/zai-coding-plan,14couples modèle/variante, startingIntelligence50, beta0.35, verification proportionate, commande ASan et critères d'acceptation du brouillon affiché. Aucun fichier global modifié, aucune nouvelle admission, aucun modèle imposé ou classifier lancé.

Contrôle réellement exécuté : fonctions readPolicyDraft et validatePolicy du plugin installé0f9902409237 extraites en mémoire et appliquées au fichier corrigé, sans import du plugin/effets natifs. **PASS**, warnings=[], classificationOverride={mode:off,missionMode:long_run},14couples, QAproportionate. Le lecteur retire bien le bloc transitoire avant validation de la politique V2. Cela ne prouve pas encore l'application runtime : le config natif doit la réaliser.

## Priorité de qualité

Instruction opérateur retenue : **la qualité prime sur la facilité**. Le mode long_run n'autorise ni faux PASS, ni suppression de contrôle, ni réutilisation de preuves périmées. L'audit doit conserver un vrai test-reviewer fresh/restrictif avec données et transport vérifiés ; le fait d'avoir proposé une tâche native dédiée ne permet pas de qualifier l'indépendance ou les permissions sans preuve. Si cette voie est incompatible, le gate reste bloqué et la clôture suspendue, même en long_run. Aucune réparation supplémentaire du harness/runtime ou campagne Linux n'est implicitement ouverte.

## État acquis et prochaine étape

Preuves H4 déjà intégrées : ASan555/555 et CPU normal484/484, revue correctness indépendante reçue ; documentation et artist-wiki actualisés. Backend de présentation arrêté. Ces résultats ne sont pas rejoués pour une simple correction de configuration ; la mission n'est pas déclarée close.

**L'utilisateur doit relancer `/ensemble-mission config`** pour présenter/appliquer le brouillon corrigé, puis son `/ensemble-mission start` pour approuver la version présentée. Le lead n'invoque pas start à sa place et ne prend pas ce message de correction pour un GO d'admission.
