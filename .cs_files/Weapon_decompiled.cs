// VampireSurvivors.Runtime, Version=0.0.0.0, Culture=neutral, PublicKeyToken=null
// VampireSurvivors.Objects.Weapons.Weapon
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Newtonsoft.Json.Linq;
using Unity.Mathematics;
using Unity.Profiling;
using Unity.Profiling.LowLevel;
using Unity.Profiling.LowLevel.Unsafe;
using UnityEngine;
using UnityEngine.Bindings;
using VampireSurvivors.Data;
using VampireSurvivors.Data.Weapons;
using VampireSurvivors.Framework;
using VampireSurvivors.Framework.TimerSystem;
using VampireSurvivors.Interfaces;
using VampireSurvivors.Objects;
using VampireSurvivors.Objects.Characters;
using VampireSurvivors.Objects.Pools;
using VampireSurvivors.Objects.Projectiles;
using VampireSurvivors.Objects.Weapons;

public class Weapon : Equipment
{
	public enum FiringAnimation
	{
		None,
		Melee,
		Ranged,
		Magic,
		Bazooka,
		GlyphAbs,
		Axe,
		ConeOfCold
	}

	[Serializable]
	private sealed class <>c
	{
		public static readonly <>c <>9;

		public static Func<Equipment, WeaponType> <>9__140_0;

		public static Func<Equipment, WeaponType> <>9__140_1;

		static <>c()
		{
			//IL_004b: Expected I, but got O
			//IL_008c: Expected I, but got O
			//IL_00bf: Unknown result type (might be due to invalid IL or missing references)
			//IL_00c4: Expected O, but got Unknown
			//IL_0038->IL0038: Incompatible stack heights: 1 vs 0
			if ((long)new string("") == 0L)
			{
				Console.WriteLine("");
				_ = 1L;
			}
			Console.WriteLine("");
			nint num = (nint)new <>c();
			object obj = new string("开瘀㐀㤀⬀㐀\u3000崀\0\u1b82Unknown call target operand: 655 Call \"il2cpp_vm_o");
			object obj2 = default(object);
			obj = obj2;
			if ((long)new string("Unmanaged memory load: [18996C9E0]") != 0L)
			{
				nint num2 = (nint)new <>c();
				object obj3 = new string("Unmanaged memory load: [1899EFB60]");
				object obj4 = (object)new string("挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䰀椀猀琀怀\u3100㰀䈀愀猀攀䈀漀搀礀㸀⤀Ⰰ\u2000嬀\u3100㠀㤀㤀㘀䄀䔀䈀㠀崀Ⰰ\u2000瘀㌀㜀") >> 12;
				object obj5 = obj4 & 0x1FFFFFL;
				object obj6 = obj5 >> 6;
				object obj7 = new string("Unmanaged memory load: [v38 @ rcx_v5+8+v43 @ rax_v6]");
			}
		}

		internal WeaponType <LevelUp>b__140_0(Equipment x)
		{
			if ((object)x != null)
			{
				return ((Equipment)this)._equipmentType;
			}
			Console.WriteLine("Method not found @180AB7550");
			WeaponType result = default(WeaponType);
			return result;
		}

		internal WeaponType <LevelUp>b__140_1(Equipment x)
		{
			if ((object)x != null)
			{
				return ((Equipment)this)._equipmentType;
			}
			Console.WriteLine("Method not found @180AB7550");
			WeaponType result = default(WeaponType);
			return result;
		}
	}

	private sealed class <>c__DisplayClass156_0
	{
		public int localIndex;

		public Weapon <>4__this;

		internal void <Fire>b__0()
		{
			//IL_0056: Expected O, but got I4
			//IL_0082: Expected O, but got I4
			//IL_0141: Expected O, but got I4
			//IL_0095->IL0095: Incompatible stack heights: 3 vs 0
			//IL_0095->IL0095: Incompatible stack heights: 3 vs 0
			//IL_010d->IL010d: Incompatible stack heights: 1 vs 0
			bool flag = (object)<>4__this == null;
			Weapon weapon2 = default(Weapon);
			Weapon weapon = weapon2;
			GameObject gameObject2 = default(GameObject);
			GameObject gameObject = gameObject2;
			object obj2 = default(object);
			object obj = obj2;
			Component component = <>4__this;
			GameObject gameObject3 = default(GameObject);
			if (!flag)
			{
				_ = <>4__this;
				_ = <>4__this;
				gameObject3 = ((Component)0).gameObject;
				bool flag2 = (object)gameObject3 == null;
				weapon = weapon2;
				gameObject = gameObject3;
				obj = 0;
				component = <>4__this;
				if (flag2)
				{
				}
			}
			Console.WriteLine("Method not found @180AB7550");
			Weapon weapon3 = weapon;
			GameObject gameObject4 = gameObject;
			object obj3 = obj;
			GameObject gameObject5;
			if ((long)new string("Unmanaged memory load: [189999291]") == 0L)
			{
				Console.WriteLine("Method not found @180AB7B30");
				Console.WriteLine("");
				gameObject5 = gameObject4;
				Console.WriteLine("");
				_ = 1L;
			}
			bool flag3 = new string("") == null;
			gameObject5 = gameObject3;
			if (!flag3)
			{
			}
			UnityEngine.Bindings.ThrowHelper.ThrowNullReferenceException(0);
		}
	}

	private sealed class <>c__DisplayClass176_0
	{
		public Weapon <>4__this;

		public FiringAnimation animation;

		internal void <PlayNextAttackAnim>b__0()
		{
			Weapon weapon = <>4__this;
			bool flag = (object)<>4__this == null;
			Weapon weapon2 = <>4__this;
			if (!flag)
			{
				bool flag2 = new string("洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䰀椀猀琀怀\u3100㰀䈀愀猀攀䈀漀搀礀㸀⤀Ⰰ\u2000瘀\u3100㈀㈀㤀\u2000䀀\u2000爀") == null;
				weapon2 = (Weapon)(object)new string("洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䰀椀猀琀怀\u3100㰀䈀愀猀攀䈀漀搀礀㸀⤀Ⰰ\u2000瘀\u3100㈀㈀㤀\u2000䀀\u2000爀");
				if (flag2)
				{
				}
			}
			Console.WriteLine("Method not found @180AB7550");
		}

		internal void <PlayNextAttackAnim>b__1()
		{
			Weapon weapon = <>4__this;
			bool flag = (object)<>4__this == null;
			Weapon weapon2 = <>4__this;
			if (!flag)
			{
				bool flag2 = new string("洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䰀椀猀琀怀\u3100㰀䈀愀猀攀䈀漀搀礀㸀⤀Ⰰ\u2000瘀\u3100㈀㈀㤀\u2000䀀\u2000爀") == null;
				weapon2 = (Weapon)(object)new string("洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䰀椀猀琀怀\u3100㰀䈀愀猀攀䈀漀搀礀㸀⤀Ⰰ\u2000瘀\u3100㈀㈀㤀\u2000䀀\u2000爀");
				if (flag2)
				{
				}
			}
			Console.WriteLine("Method not found @180AB7550");
		}

		internal void <PlayNextAttackAnim>b__2()
		{
			Weapon weapon = <>4__this;
			bool flag = (object)<>4__this == null;
			Weapon weapon2 = <>4__this;
			if (!flag)
			{
				bool flag2 = new string("洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䰀椀猀琀怀\u3100㰀䈀愀猀攀䈀漀搀礀㸀⤀Ⰰ\u2000瘀\u3100㈀㈀㤀\u2000䀀\u2000爀") == null;
				weapon2 = (Weapon)(object)new string("洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䰀椀猀琀怀\u3100㰀䈀愀猀攀䈀漀搀礀㸀⤀Ⰰ\u2000瘀\u3100㈀㈀㤀\u2000䀀\u2000爀");
				if (flag2)
				{
				}
			}
			Console.WriteLine("Method not found @180AB7550");
		}

		internal void <PlayNextAttackAnim>b__3()
		{
			Weapon weapon = <>4__this;
			bool flag = (object)<>4__this == null;
			Weapon weapon2 = <>4__this;
			if (!flag)
			{
				bool flag2 = new string("") == null;
				weapon2 = (Weapon)(object)new string("");
				if (flag2)
				{
				}
			}
			Console.WriteLine("Method not found @180AB7550");
		}
	}

	private Projectile _ProjectilePrefab;

	protected GameManager _gameMan;

	protected PlayerOptions _playerOptions;

	protected GameSessionData _gameSessionData;

	protected WeaponData _currentWeaponData;

	protected bool _skipAddingEvolution;

	protected readonly List<Projectile> _spawnedProjectiles;

	protected Transform _cachedTransform;

	protected Timer _lastShotTimer;

	protected Timer _firingTimer;

	private Timer _firingAnimEvent;

	protected Transform _targetTransform;

	protected BulletPool _projectilePool;

	protected int _critIndex;

	protected List<float> _critChancesArray;

	protected int _bounces;

	protected int _bonusBounces;

	protected float _lastFiringInterval;

	protected bool _beginningArcana;

	protected int _beginningAmount;

	protected List<Collider> _wallsColliders;

	protected bool _isVisible;

	protected WeaponType _explosionType;

	[NonSerialized]
	public bool _explodeOnExpire;

	protected BulletPool _secondaryPool;

	protected ProjectileFactory _projectileFactory;

	protected WeaponType _secondaryOvarlapDamageType;

	public LimitBreakData accumulatedLimitBreaks;

	[NonSerialized]
	public bool IsHoming;

	[NonSerialized]
	public bool IsAdept;

	public bool HasCooldownSpeedBonus;

	private float <StatsInflictedDamage>k__BackingField;

	private float <StatsLifetime>k__BackingField;

	private bool <CanCrit>k__BackingField;

	private float <FreezeChance>k__BackingField;

	private float _defangChance;

	private float <TotalTime>k__BackingField;

	private int <LimitBreakLevel>k__BackingField;

	private bool <SkipAddingNormalWeapon>k__BackingField;

	private bool <ShowAsDisabledOnEquipmentPanel>k__BackingField;

	private static readonly ProfilerMarker _markerCleanup;

	private static readonly ProfilerMarker _markerFireOneProjectile;

	protected virtual int ProjectilePoolSize => 50;

	public PhysicsGroup ProjectileGroup => _projectilePool;

	public List<Projectile> SpawnedProjectiles => _spawnedProjectiles;

	public GameManager GameMan => _gameMan;

	protected HitVfxType VfxType
	{
		get
		{
			//IL_002b: Expected I4, but got I8
			if (_currentWeaponData != null)
			{
				return ((WeaponData)(object)this).<hitVFX>k__BackingField;
			}
			return HitVfxType.Default;
		}
	}

	protected virtual bool UseOnlineTimer => true;

	public float StatsInflictedDamage
	{
		get
		{
			return <StatsInflictedDamage>k__BackingField;
		}
		set
		{
			float num = default(float);
			<StatsInflictedDamage>k__BackingField = num;
		}
	}

	public float StatsLifetime
	{
		get
		{
			return <StatsLifetime>k__BackingField;
		}
		private set
		{
			float num = default(float);
			<StatsLifetime>k__BackingField = num;
		}
	}

	public virtual float Chance
	{
		get
		{
			if (_currentWeaponData != null)
			{
				return ((WeaponData)(object)this).<chance>k__BackingField;
			}
			Console.WriteLine("Method not found @180AB7550");
			float result = default(float);
			return result;
		}
	}

	public int Penetrating
	{
		get
		{
			if (_currentWeaponData != null)
			{
				return ((WeaponData)(object)this).<penetrating>k__BackingField;
			}
			Console.WriteLine("Method not found @180AB7550");
			int result = default(int);
			return result;
		}
		protected set
		{
			if (_currentWeaponData != null)
			{
				WeaponData weaponData = default(WeaponData);
				weaponData.<penetrating>k__BackingField = value;
			}
			else
			{
				Console.WriteLine("Method not found @180AB7550");
			}
		}
	}

	public float Interval
	{
		get
		{
			if (_currentWeaponData != null)
			{
				return ((WeaponData)(object)this).<interval>k__BackingField;
			}
			Console.WriteLine("Method not found @180AB7550");
			float result = default(float);
			return result;
		}
		set
		{
			if (_currentWeaponData != null)
			{
				WeaponData weaponData = default(WeaponData);
				float <interval>k__BackingField = default(float);
				weaponData.<interval>k__BackingField = <interval>k__BackingField;
			}
			else
			{
				Console.WriteLine("Method not found @180AB7550");
			}
		}
	}

	protected float Duration
	{
		get
		{
			/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
		}
	}

	public float RepeatInterval
	{
		get
		{
			if (_currentWeaponData != null)
			{
				return ((WeaponData)(object)this).<repeatInterval>k__BackingField;
			}
			Console.WriteLine("Method not found @180AB7550");
			float result = default(float);
			return result;
		}
	}

	public WeaponData CurrentWeaponData => _currentWeaponData;

	public float HitBoxDelay
	{
		get
		{
			/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
		}
	}

	public float Knockback
	{
		get
		{
			/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
		}
	}

	public PlayerOptions PlayerOptions => _playerOptions;

	public bool CanCrit
	{
		get
		{
			return <CanCrit>k__BackingField;
		}
		protected set
		{
			<CanCrit>k__BackingField = value;
		}
	}

	public List<float> CritChancesArray => _critChancesArray;

	public float FreezeChance
	{
		get
		{
			return <FreezeChance>k__BackingField;
		}
		set
		{
			float num = default(float);
			<FreezeChance>k__BackingField = num;
		}
	}

	public virtual float DefangChance
	{
		get
		{
			return _defangChance;
		}
		set
		{
			float defangChance = default(float);
			_defangChance = defangChance;
		}
	}

	public int CritIndex
	{
		get
		{
			return _critIndex;
		}
		set
		{
			_critIndex = value;
		}
	}

	protected Vector2 PlayerPos
	{
		get
		{
			if (new string("") != null)
			{
				new string("");
				new string("");
				object obj = default(object);
				float2 position = ((ArcadeSprite)obj).position;
				Vector2 result = default(Vector2);
				return result;
			}
			Console.WriteLine("Method not found @180AB7550");
			Vector2 result2 = default(Vector2);
			return result2;
		}
	}

	public float TotalTime
	{
		get
		{
			return <TotalTime>k__BackingField;
		}
		set
		{
			float num = default(float);
			<TotalTime>k__BackingField = num;
		}
	}

	public int LimitBreakLevel
	{
		get
		{
			return <LimitBreakLevel>k__BackingField;
		}
		private set
		{
			<LimitBreakLevel>k__BackingField = value;
		}
	}

	public bool SkipAddingEvolution
	{
		get
		{
			return _skipAddingEvolution;
		}
		set
		{
			_skipAddingEvolution = value;
		}
	}

	public bool SkipAddingNormalWeapon
	{
		get
		{
			return <SkipAddingNormalWeapon>k__BackingField;
		}
		set
		{
			<SkipAddingNormalWeapon>k__BackingField = value;
		}
	}

	public bool IsVisible => _isVisible;

	public bool ShowAsDisabledOnEquipmentPanel
	{
		get
		{
			return <ShowAsDisabledOnEquipmentPanel>k__BackingField;
		}
		set
		{
			<ShowAsDisabledOnEquipmentPanel>k__BackingField = value;
		}
	}

	public virtual float HeartOfFirePower
	{
		get
		{
			if (_currentWeaponData != null)
			{
				return ((WeaponData)(object)this).<power>k__BackingField;
			}
			Console.WriteLine("Method not found @180AB7550");
			float result = default(float);
			return result;
		}
	}

	public override bool IsPowerup()
	{
		return false;
	}

	public virtual float StatsGetDps()
	{
		//IL_000b: Invalid comparison between F4 and I4
		//IL_0051: Expected F4, but got I4
		//IL_0039: Invalid comparison between F4 and I4
		bool flag = <StatsLifetime>k__BackingField == 0f;
		Console.WriteLine("");
		if (!flag)
		{
			Console.WriteLine("氀愀猀猀㰀唀渀椀琀礀䔀渀最椀渀攀⸀伀戀樀攀挀琀㸀⤀⬀䔀㐀崀\0▁Un");
			if (<StatsInflictedDamage>k__BackingField != 0f)
			{
				return <StatsInflictedDamage>k__BackingField / <StatsLifetime>k__BackingField;
			}
		}
		return 0f;
	}

	protected override void FakeConstruct()
	{
		//IL_0079: Expected I, but got O
		//IL_0183: Expected I, but got O
		//IL_02c9: Expected I, but got O
		//IL_0289: Unknown result type (might be due to invalid IL or missing references)
		//IL_028e: Expected O, but got Unknown
		//IL_02ad: Unknown result type (might be due to invalid IL or missing references)
		//IL_02b2: Expected O, but got Unknown
		//IL_0425: Expected I, but got O
		//IL_050b: Expected I, but got O
		//IL_04cb: Unknown result type (might be due to invalid IL or missing references)
		//IL_04d0: Expected O, but got Unknown
		//IL_04ef: Unknown result type (might be due to invalid IL or missing references)
		//IL_04f4: Expected O, but got Unknown
		//IL_03e5: Unknown result type (might be due to invalid IL or missing references)
		//IL_03ea: Expected O, but got Unknown
		//IL_0409: Unknown result type (might be due to invalid IL or missing references)
		//IL_040e: Expected O, but got Unknown
		//IL_06a1: Expected I, but got O
		//IL_0661: Unknown result type (might be due to invalid IL or missing references)
		//IL_0666: Expected O, but got Unknown
		//IL_0685: Unknown result type (might be due to invalid IL or missing references)
		//IL_068a: Expected O, but got Unknown
		//IL_0802: Expected I, but got O
		//IL_07c2: Unknown result type (might be due to invalid IL or missing references)
		//IL_07c7: Expected O, but got Unknown
		//IL_07e6: Unknown result type (might be due to invalid IL or missing references)
		//IL_07eb: Expected O, but got Unknown
		//IL_0913: Unknown result type (might be due to invalid IL or missing references)
		//IL_0918: Expected O, but got Unknown
		//IL_0038->IL0038: Incompatible stack heights: 1 vs 0
		//IL_0070->IL0070: Incompatible stack heights: 1 vs 0
		//IL_0113->IL0113: Incompatible stack heights: 1 vs 0
		//IL_01d1->IL0113: Incompatible stack heights: 1 vs 0
		//IL_0317->IL0113: Incompatible stack heights: 3 vs 0
		//IL_0264->IL01d1: Incompatible stack heights: 4 vs 1
		//IL_02c0->IL02c0: Incompatible stack heights: 4 vs 3
		//IL_0559->IL0113: Incompatible stack heights: 5 vs 0
		//IL_03c0->IL032d: Incompatible stack heights: 6 vs 3
		//IL_0502->IL0502: Incompatible stack heights: 6 vs 5
		//IL_041c->IL041c: Incompatible stack heights: 6 vs 5
		//IL_058e->IL047e: Incompatible stack heights: 6 vs 5
		//IL_06ef->IL0113: Incompatible stack heights: 7 vs 0
		//IL_0640->IL058e: Incompatible stack heights: 8 vs 6
		//IL_0698->IL0698: Incompatible stack heights: 8 vs 7
		//IL_0850->IL0113: Incompatible stack heights: 8 vs 0
		//IL_07a1->IL0705: Incompatible stack heights: 9 vs 7
		//IL_07f9->IL07f9: Incompatible stack heights: 9 vs 8
		//IL_08f2->IL0866: Incompatible stack heights: 10 vs 8
		//IL_0938->IL0938: Incompatible stack heights: 10 vs 9
		if ((long)new string("攀⸀伀戀樀攀挀琀⤀Ⰰ\u2000瘀㤀㌀\u2000䀀\u2000爀\u3100㐀开瘀㌀Ⰰ\u2000\u3000Ⰰ\u2000瘀㤀㘀\u2000䀀\u2000爀㤀开瘀㈀\0㦁Unknown call t") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 17 Call \"il2cpp_codegen_initialize_runtime_metadata\", v16 @ rax_v1, typeof(VampireSurvivors.Framework.GM), methodInfo @ rdx, v18 @ r8, v19 @ r9");
			_ = 1L;
		}
		if ((long)new string("Unmanaged memory load: [1899A3788]") == 0L)
		{
			Console.WriteLine("㤀开瘀㈀\0㦁Unknown call target operand: 436 Call \"il2cpp_codeg");
			_ = 1L;
		}
		nint num = (nint)new GM();
		object obj = new string("Unmanaged memory load: [v43 @ rax_v5 (Il2CppClass<VampireSurvivors.Framework.GM>)+B8]");
		object obj2 = obj;
		bool flag = obj == null;
		object obj4 = default(object);
		object obj3 = obj4;
		object obj6 = default(object);
		object obj5 = obj6;
		object obj8 = default(object);
		object obj7 = obj8;
		object obj9 = obj;
		object obj10;
		object obj30 = default(object);
		Weapon weapon = default(Weapon);
		object obj32;
		object obj33;
		if (!flag)
		{
			obj10 = new string("Unmanaged memory load: [1899A9880]");
			new string("");
			bool flag2 = new string("Unmanaged memory load: [18996C9E0]") == null;
			object obj11 = obj4;
			object obj12 = new string("Unmanaged memory load: [18996C9E0]");
			if (flag2)
			{
				nint num2 = (nint)new GM();
				object obj13 = new string("");
				object obj14 = obj13;
				bool flag3 = obj13 == null;
				obj3 = obj11;
				obj5 = obj12;
				obj7 = obj10;
				obj9 = obj13;
				if (!flag3)
				{
					object obj20 = default(object);
					object obj15;
					object obj16;
					while (true)
					{
						Console.WriteLine("Not implemented instruction: bts rcx,r8");
						bool flag4 = (object)new string("") != new string("");
						new string("");
						bool flag5 = obj12 == null;
						obj15 = obj11;
						obj16 = obj12;
						if (flag5)
						{
							break;
						}
						new string("");
						if ((object)new string("") == new string(""))
						{
							object obj17 = (object)new string("") >> 12;
							object obj18 = obj17 & 0x1FFFFFL;
							object obj19 = obj18 >> 6;
							obj20 = obj18 & 0x3FL;
							object obj21 = new string("");
							break;
						}
					}
					nint num3 = (nint)new GM();
					object obj22 = new string("椀氀㈀挀瀀瀀开爀甀渀琀椀洀攀开挀氀");
					object obj23 = obj22;
					bool flag6 = obj22 == null;
					obj3 = obj15;
					obj5 = obj16;
					obj7 = obj10;
					obj9 = obj22;
					if (!flag6)
					{
						object obj24 = default(object);
						obj11 = obj24;
						obj12 = new string("Unmanaged memory load: [18996C9E0]");
						object obj25;
						object obj26;
						while (true)
						{
							Console.WriteLine("Not implemented instruction: bts rcx,r8");
							bool flag7 = (object)new string("") != new string("");
							new string("㜀㐀㐀\u2000䀀\u2000爀愀砀开瘀㌀㜀\u2000⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀䘀爀愀洀攀眀漀爀欀⸀倀栀愀猀攀爀⸀倀栀愀猀攀爀匀瀀爀椀");
							bool flag8 = obj16 == null;
							obj25 = obj15;
							obj26 = obj16;
							if (flag8)
							{
								break;
							}
							new string("");
							if ((object)new string("") == new string(""))
							{
								object obj27 = (object)new string("倀栀愀猀攀爀⸀倀栀愀猀攀爀匀瀀爀椀琀攀⤀⬀\u3100\u3000") >> 12;
								object obj28 = obj27 & 0x1FFFFFL;
								object obj29 = obj28 >> 6;
								obj30 = obj28 & 0x3FL;
								object obj31 = new string("");
								break;
							}
						}
						nint num4 = (nint)new GM();
						object gameMan = new string("漀愀搀㨀\u2000嬀瘀\u3100㜀\u3000\u2000䀀\u2000爀愀砀开瘀㜀\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀䘀爀愀洀");
						weapon._gameMan = (GameManager)gameMan;
						bool flag9 = obj26 == null;
						obj32 = obj25;
						obj33 = obj26;
						if (!flag9)
						{
							obj15 = obj20;
							obj16 = new string("Unmanaged memory load: [18996C9E0]");
							goto IL_047e;
						}
						goto IL_0502;
					}
				}
			}
		}
		goto IL_0113;
		IL_047e:
		Console.WriteLine("Not implemented instruction: bts rcx,r8");
		bool flag10 = (object)new string("愀搀㨀\u2000嬀瘀㔀㜀\u2000䀀\u2000爀愀砀开瘀㠀\u2000⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀倀氀愀礀攀爀伀瀀琀椀漀") != new string("愀搀㨀\u2000嬀瘀㔀㜀\u2000䀀\u2000爀愀砀开瘀㠀\u2000⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀倀氀愀礀攀爀伀瀀琀椀漀");
		object obj34 = (object)_gameMan >> 12;
		object obj35 = obj34 & 0x1FFFFFL;
		object obj36 = obj35 >> 6;
		object obj37 = obj35 & 0x3FL;
		object obj38 = new string("氀愀礀攀爀伀瀀琀椀漀渀猀⬀伀渀嘀愀氀甀攀䌀栀愀渀最攀搀⤀⬀㤀\u3000崀\0꾀Unmana");
		goto IL_0502;
		IL_0113:
		Console.WriteLine("Method not found @180AB7550");
		return;
		IL_0502:
		nint num5 = (nint)new GM();
		object obj39 = new string("");
		object obj40 = obj39;
		bool flag11 = obj39 == null;
		obj3 = obj32;
		obj5 = obj33;
		obj7 = obj10;
		obj9 = obj39;
		if (!flag11)
		{
			new string("愀搀㨀\u2000嬀瘀㔀㜀\u2000䀀\u2000爀愀砀开瘀㠀\u2000⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀倀氀愀礀攀爀伀瀀琀椀漀");
			if ((object)new string("愀搀㨀\u2000嬀瘀㔀㜀\u2000䀀\u2000爀愀砀开瘀㠀\u2000⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀倀氀愀礀攀爀伀瀀琀椀漀") != new string("愀搀㨀\u2000嬀瘀㔀㜀\u2000䀀\u2000爀愀砀开瘀㠀\u2000⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀倀氀愀礀攀爀伀瀀琀椀漀"))
			{
				goto IL_047e;
			}
			object obj46 = default(object);
			object obj41;
			object obj42;
			while (true)
			{
				Console.WriteLine("Not implemented instruction: bts rcx,r8");
				bool flag12 = (object)new string("") != new string("");
				weapon._playerOptions = (PlayerOptions)(object)new string("");
				bool flag13 = obj33 == null;
				obj41 = obj32;
				obj42 = obj33;
				if (flag13)
				{
					break;
				}
				object obj25 = obj30;
				object obj26 = new string("Unmanaged memory load: [18996C9E0]");
				new string("");
				if ((object)new string("") == new string(""))
				{
					object obj43 = (object)_playerOptions >> 12;
					object obj44 = obj43 & 0x1FFFFFL;
					object obj45 = obj44 >> 6;
					obj46 = obj44 & 0x3FL;
					object obj47 = new string("");
					break;
				}
			}
			nint num6 = (nint)new GM();
			object obj48 = new string("㨀\u2000\u3100㤀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开挀漀搀攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀");
			object obj49 = obj48;
			bool flag14 = obj48 == null;
			obj3 = obj41;
			obj5 = obj42;
			obj7 = obj10;
			obj9 = obj48;
			if (!flag14)
			{
				obj32 = obj37;
				obj33 = new string("Unmanaged memory load: [18996C9E0]");
				object obj50;
				object obj51;
				while (true)
				{
					Console.WriteLine("Not implemented instruction: bts rcx,r8");
					bool flag15 = (object)new string("") != new string("");
					weapon._gameSessionData = (GameSessionData)(object)new string("");
					bool flag16 = obj42 == null;
					obj50 = obj41;
					obj51 = obj42;
					if (flag16)
					{
						break;
					}
					new string("");
					if ((object)new string("") == new string(""))
					{
						object obj52 = (object)_gameSessionData >> 12;
						object obj53 = obj52 & 0x1FFFFFL;
						object obj54 = obj53 >> 6;
						object obj55 = obj53 & 0x3FL;
						object obj56 = new string("眀渀\u2000挀愀氀氀\u2000琀愀爀最攀琀\u2000漀瀀攀爀愀渀搀㨀\u2000㈀㈀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开挀漀搀攀最攀渀开椀渀椀琀椀愀氀");
						break;
					}
				}
				nint num7 = (nint)new GM();
				object obj57 = new string("氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㌀㘀\u2000䀀\u2000爀愀砀开瘀㈀Ⰰ\u2000洀攀琀栀漀搀漀昀⠀嘀");
				object obj58 = obj57;
				bool flag17 = obj57 == null;
				obj3 = obj50;
				obj5 = obj51;
				obj7 = obj10;
				obj9 = obj57;
				if (!flag17)
				{
					obj41 = obj46;
					obj42 = new string("Unmanaged memory load: [18996C9E0]");
					while (true)
					{
						Console.WriteLine("Not implemented instruction: bts rcx,r8");
						bool flag18 = (object)new string("") != new string("");
						weapon._projectileFactory = (ProjectileFactory)(object)new string("");
						if (obj51 != null)
						{
							new string("");
							if ((object)new string("") == new string(""))
							{
								object obj59 = (object)_projectileFactory >> 12;
								object obj60 = obj59 & 0x1FFFFFL;
								object obj61 = obj60 >> 6;
								object obj62 = new string("洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀瘀㌀㜀\u2000䀀\u2000爀愀砀开瘀㔀\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀");
								break;
							}
							continue;
						}
						break;
					}
					return;
				}
			}
		}
		goto IL_0113;
	}

	protected virtual void Awake()
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	protected override void OnDestroy()
	{
		//IL_003a: Expected O, but got I4
		//IL_0074: Expected O, but got I4
		//IL_00e7: Expected O, but got I8
		//IL_00ae: Expected O, but got I4
		//IL_0111: Unknown result type (might be due to invalid IL or missing references)
		//IL_0116: Expected O, but got Unknown
		//IL_01c7: Expected O, but got I8
		//IL_019c: Expected O, but got I4
		//IL_0212: Unknown result type (might be due to invalid IL or missing references)
		//IL_0217: Expected O, but got Unknown
		//IL_0236: Unknown result type (might be due to invalid IL or missing references)
		//IL_023b: Expected O, but got Unknown
		//IL_003a->IL003a: Incompatible stack heights: 2 vs 0
		//IL_0074->IL0074: Incompatible stack heights: 2 vs 0
		//IL_00ae->IL00ae: Incompatible stack heights: 2 vs 0
		//IL_019c->IL019c: Incompatible stack heights: 3 vs 0
		//IL_0205->IL0150: Incompatible stack heights: 1 vs 0
		//IL_025b->IL025b: Incompatible stack heights: 1 vs 0
		if (_firingAnimEvent != null)
		{
			_ = _firingAnimEvent;
			Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.TimerSystem.Timer::Cancel");
			_ = _firingAnimEvent;
			((Timer)0).Cancel();
		}
		if (_firingTimer != null)
		{
			_ = _firingTimer;
			Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.TimerSystem.Timer::Cancel");
			_ = _firingTimer;
			((Timer)0).Cancel();
		}
		if (_projectilePool != null)
		{
			_ = _projectilePool;
			Console.WriteLine("");
			_ = _projectilePool;
			((BulletPool)0).Destroy();
		}
		bool flag = (long)new string("Unmanaged memory load: [18996C9E0]") == 0L;
		object obj = new string("Unmanaged memory load: [1899A9880]");
		Weapon weapon = default(Weapon);
		weapon._projectilePool = (BulletPool)0L;
		if (!flag)
		{
			object obj2 = (object)_projectilePool >> 12;
			object obj3 = obj2 & 0x1FFFFFL;
			object obj4 = obj3 >> 6;
			object obj5 = new string("渀欀渀漀眀渀\u2000挀愀氀氀\u2000琀愀爀最攀琀\u2000漀瀀攀爀愀渀搀㨀\u2000㈀\u3000\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀");
		}
		if (_secondaryPool != null)
		{
			goto IL_0150;
		}
		goto IL_019c;
		IL_0150:
		Console.WriteLine("Not implemented instruction: bts rcx,r8");
		bool flag2 = (object)new string("渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㔀㔀\u2000䀀\u2000爀愀砀开瘀㈀Ⰰ\u2000洀攀琀栀漀搀漀昀⠀") != new string("渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㔀㔀\u2000䀀\u2000爀愀砀开瘀㈀Ⰰ\u2000洀攀琀栀漀搀漀昀⠀");
		_ = _secondaryPool;
		Console.WriteLine("");
		_ = _secondaryPool;
		((BulletPool)0).Destroy();
		goto IL_019c;
		IL_019c:
		bool flag3 = (long)new string("Unmanaged memory load: [18996C9E0]") == 0L;
		weapon._secondaryPool = (BulletPool)0L;
		if (!flag3)
		{
			new string("渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㔀㔀\u2000䀀\u2000爀愀砀开瘀㈀Ⰰ\u2000洀攀琀栀漀搀漀昀⠀");
			if ((object)new string("渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㔀㔀\u2000䀀\u2000爀愀砀开瘀㈀Ⰰ\u2000洀攀琀栀漀搀漀昀⠀") == new string("渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㔀㔀\u2000䀀\u2000爀愀砀开瘀㈀Ⰰ\u2000洀攀琀栀漀搀漀昀⠀"))
			{
				Weapon weapon2 = default(Weapon);
				object obj6 = weapon2 + 272L;
				object obj7 = obj6 >> 12;
				object obj8 = obj7 & 0x1FFFFFL;
				object obj9 = obj8 >> 6;
				object obj10 = new string("攀琀栀漀搀漀昀⠀匀礀猀琀攀洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䰀椀猀琀怀\u3100㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀");
				return;
			}
			goto IL_0150;
		}
	}

	public virtual void InitWeapon(VampireSurvivors.Objects.Characters.CharacterController characterController, WeaponType weaponType)
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	public virtual void OnMirrorData(Vector2 position)
	{
	}

	public virtual void OnWeaponAdded()
	{
	}

	public virtual float CalculateTotalDamage()
	{
		return <StatsInflictedDamage>k__BackingField;
	}

	protected virtual void OnStart()
	{
		//IL_0044: Unknown result type (might be due to invalid IL or missing references)
		//IL_0049: Expected O, but got Unknown
		//IL_0092: Unknown result type (might be due to invalid IL or missing references)
		//IL_0097: Expected O, but got Unknown
		//IL_00ae: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b3: Expected O, but got Unknown
		//IL_00d9: Expected O, but got I4
		//IL_0196: Expected I, but got O
		//IL_0135: Unknown result type (might be due to invalid IL or missing references)
		//IL_013a: Expected O, but got Unknown
		//IL_0151: Unknown result type (might be due to invalid IL or missing references)
		//IL_0156: Expected O, but got Unknown
		//IL_0177: Unknown result type (might be due to invalid IL or missing references)
		//IL_017c: Expected O, but got Unknown
		//IL_0297: Unknown result type (might be due to invalid IL or missing references)
		//IL_029c: Expected O, but got Unknown
		//IL_02b3: Unknown result type (might be due to invalid IL or missing references)
		//IL_02b8: Expected O, but got Unknown
		//IL_01e9: Unknown result type (might be due to invalid IL or missing references)
		//IL_01ee: Expected O, but got Unknown
		//IL_0205: Unknown result type (might be due to invalid IL or missing references)
		//IL_020a: Expected O, but got Unknown
		//IL_0325: Unknown result type (might be due to invalid IL or missing references)
		//IL_032a: Expected O, but got Unknown
		//IL_0341: Unknown result type (might be due to invalid IL or missing references)
		//IL_0346: Expected O, but got Unknown
		//IL_00d9->IL00d9: Incompatible stack heights: 2 vs 0
		//IL_018d->IL018d: Incompatible stack heights: 1 vs 0
		bool flag = (long)new string("") != 0L;
		ArcadePhysicsCallback arcadePhysicsCallback2 = default(ArcadePhysicsCallback);
		ArcadePhysicsCallback arcadePhysicsCallback = arcadePhysicsCallback2;
		object obj = default(object);
		if (!flag)
		{
			Console.WriteLine("");
			obj = arcadePhysicsCallback2 | 0L;
			Console.WriteLine("");
		}
		ProfilerMarker markerCleanup = _markerCleanup;
		Console.WriteLine("椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㔀㈀\u3100\u2000䀀\u2000爀愀砀开瘀㔀Ⰰ\u2000琀礀");
		object obj3 = default(object);
		if (new string("") != null)
		{
			object obj2 = obj | 0L;
			Console.WriteLine("");
			obj3 = obj2 | 0L;
			Console.WriteLine("");
			Console.WriteLine("");
			((Weapon)0).PlayNextAttackAnim();
		}
		ProfilerMarker markerCleanup2 = _markerCleanup;
		object obj4 = new string("");
		Console.WriteLine("");
		bool flag2 = (long)new string("Unmanaged memory load: [18996AD90]") != 0L;
		ArcadePhysicsCallback arcadePhysicsCallback3 = arcadePhysicsCallback;
		object obj6 = default(object);
		if (!flag2)
		{
			object obj5 = obj3 | 0L;
			Console.WriteLine("攀⸀嘀攀挀琀漀爀㈀⤀Ⰰ\u2000瘀\u3100㠀\u2000䀀\u2000爀㠀\u2000⠀匀礀猀琀攀洀⸀匀琀爀椀渀最⤀Ⰰ\u2000瘀\u3100㤀\u2000䀀\u2000爀㤀\u2000⠀匀礀猀琀攀洀⸀");
			obj6 = obj5 | 0L;
			Console.WriteLine("\u2000䀀\u2000爀㠀\u2000⠀匀礀猀琀攀洀⸀匀琀爀椀渀最⤀Ⰰ\u2000瘀\u3100㤀\u2000䀀");
			Console.WriteLine("渀最⤀\0ꞀUnknown operand: Method: VampireSurvivors.Objec");
			ArcadePhysicsCallback arcadePhysicsCallback4 = (ArcadePhysicsCallback)(arcadePhysicsCallback | 0L);
			_ = 1L;
			arcadePhysicsCallback3 = arcadePhysicsCallback4;
		}
		nint num = (nint)new ArcadePhysics();
		bool flag3 = ((Factory)(object)this)._world == null;
		ArcadePhysicsCallback arcadePhysicsCallback6 = default(ArcadePhysicsCallback);
		ArcadePhysicsCallback arcadePhysicsCallback5 = arcadePhysicsCallback6;
		ArcadeColliderType arcadeColliderType2 = default(ArcadeColliderType);
		ArcadeColliderType arcadeColliderType = arcadeColliderType2;
		ArcadeColliderType arcadeColliderType3 = (ArcadeColliderType)(object)new string("瘀\u3100\u3000㠀\u2000䀀\u2000爀\u3100㐀开瘀㈀⬀㐀㘀㈀䔀\u3000⬀瘀㘀㐀㌀\u2000䀀\u2000爀搀砀开瘀\u3100\u3100\u2000⠀匀礀猀琀攀洀⸀䈀漀");
		Factory factory = (Factory)(object)new string("漀瀀攀爀愀渀搀㨀\u2000㈀㔀㌀\u2000䌀愀氀");
		object obj8 = default(object);
		if (!flag3)
		{
			object obj7 = obj6 | 0L;
			Console.WriteLine("");
			obj8 = obj7 | 0L;
			Console.WriteLine("挀愀氀氀\u2000琀愀爀最攀琀\u2000漀瀀攀爀");
			Factory scene = (Factory)(object)((World)(object)this)._scene;
			bool flag4 = ((World)(object)this)._scene == null;
			arcadePhysicsCallback5 = arcadePhysicsCallback6;
			arcadeColliderType = arcadeColliderType2;
			arcadeColliderType3 = (ArcadeColliderType)(object)new string("瘀\u3100\u3000㠀\u2000䀀\u2000爀\u3100㐀开瘀㈀⬀㐀㘀㈀䔀\u3000⬀瘀㘀㐀㌀\u2000䀀\u2000爀搀砀开瘀\u3100\u3100\u2000⠀匀礀猀琀攀洀⸀䈀漀");
			factory = (Factory)(object)((World)(object)this)._scene;
			if (flag4)
			{
			}
		}
		object obj10;
		bool flag5;
		do
		{
			Console.WriteLine("Method not found @180AB7550");
			ArcadePhysicsCallback arcadePhysicsCallback7 = arcadePhysicsCallback5;
			ArcadeColliderType arcadeColliderType4 = arcadeColliderType;
			ArcadeColliderType arcadeColliderType5 = arcadeColliderType3;
			Factory factory2 = factory;
			object obj9 = obj8 | 0L;
			Console.WriteLine("瀀瀀䌀氀愀猀猀㰀䄀爀挀愀搀攀倀栀礀猀椀挀猀㸀⤀⬀䈀㠀崀\0啕渀洀愀渀");
			obj10 = obj9 | 0L;
			Console.WriteLine("最攀琀\u2000漀瀀攀爀愀渀搀㨀\u2000㐀\u3000㔀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开挀漀搀攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开");
			flag5 = (object)_gameMan == null;
			arcadePhysicsCallback5 = arcadePhysicsCallback6;
			arcadeColliderType = arcadeColliderType2;
			arcadeColliderType3 = (ArcadeColliderType)(object)new string("瘀\u3100\u3000㠀\u2000䀀\u2000爀\u3100㐀开瘀㈀⬀㐀㘀㈀䔀\u3000⬀瘀㘀㐀㌀\u2000䀀\u2000爀搀砀开瘀\u3100\u3100\u2000⠀匀礀猀琀攀洀⸀䈀漀");
			factory = (Factory)(object)((World)(object)this)._scene;
		}
		while (flag5);
		Console.WriteLine("Method not found @180AB7550");
		Console.WriteLine("Method not found @180AB7540");
		object obj11 = obj10 | 0L;
		Console.WriteLine("嬀瘀\u3100㘀\u3100\u2000䀀\u2000爀搀砀开瘀\u3100㠀\u2000");
		object obj12 = obj11 | 0L;
		Console.WriteLine("氀猀⸀刀攀渀搀攀爀椀渀最䔀砀琀攀渀猀椀漀渀猀㸀⤀⬀䔀㐀崀\0햁Unknown call target opera");
		Console.WriteLine("㈀⤀Ⰰ\u2000瘀\u3100㐀㤀\u2000䀀\u2000爀㠀开瘀\u3100㘀\u2000⠀匀礀猀琀攀洀⸀匀琀爀椀渀最⤀Ⰰ\u2000嬀瘀\u3100㠀㔀\u2000䀀\u2000爀愀砀开瘀㌀㜀⬀\u3100\u3000崀\0啕渀");
		ProfilerMarker markerCleanup3 = _markerCleanup;
		Console.WriteLine("䀀\u2000爀㤀\u2000⠀匀礀猀琀攀洀⸀䈀漀漀");
	}

	public virtual float2 GetFiringVector()
	{
		if (new string("") != null)
		{
			Console.WriteLine("Method not found @185015910");
			float2 result = default(float2);
			return result;
		}
		Console.WriteLine("Method not found @180AB7550");
		float2 result2 = default(float2);
		return result2;
	}

	protected virtual bool OnBulletOverlapsEnemy(CallbackContext context, ArcadeColliderType second, ArcadeColliderType first)
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	protected virtual bool OnBulletOverlapsPlayer(CallbackContext context, ArcadeColliderType second, ArcadeColliderType first)
	{
		//IL_0024: Expected I, but got O
		//IL_0060: Expected I, but got O
		//IL_0068: Expected O, but got I
		//IL_00c1: Expected O, but got I8
		bool flag = (long)new string("") != 0L;
		Weapon weapon = default(Weapon);
		nint num = (nint)weapon;
		if (!flag)
		{
			Console.WriteLine("");
			Console.WriteLine("猀漀氀瘀攀搀\u2000戀攀昀漀爀攀\u2000䤀䰀\u2000最攀渀⤀\0얀Unmanaged memory load: [v13");
		}
		bool flag2 = first == null;
		ArcadeColliderType arcadeColliderType = second;
		nint num2 = (nint)context;
		HashSet<object> hashSet = (HashSet<object>)num;
		if (!flag2)
		{
			Console.WriteLine("");
			Console.WriteLine("⤀Ⰰ\u2000眀攀愀瀀漀渀猀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䰀椀猀琀怀\u3100㰀匀礀猀琀攀洀");
			Console.WriteLine("Method not found @180002A60");
			HashSet<object> hashSet2 = default(HashSet<object>);
			bool flag3 = hashSet2 == null;
			arcadeColliderType = first;
			Console.WriteLine("渀椀琀礀⸀倀爀漀昀椀氀椀渀最⸀倀爀漀昀椀氀攀爀䴀愀爀欀攀爀⤀\0門Unmanaged ");
			num2 = (nint)/*Error near IL_00b4: Stack underflow*/;
			hashSet = (HashSet<object>)3L;
			if (flag3)
			{
			}
		}
		Console.WriteLine("Method not found @180AB7550");
		bool result = default(bool);
		return result;
	}

	protected virtual bool OnSecondaryBulletOverlapsEnemy(CallbackContext context, ArcadeColliderType second, ArcadeColliderType first)
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	protected virtual bool OnSecondaryBulletOverlapsEnemyCurse(CallbackContext context, ArcadeColliderType second, ArcadeColliderType first)
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	protected virtual bool OnBulletOverlapsEnemyRetaliation(CallbackContext context, ArcadeColliderType second, ArcadeColliderType first)
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	protected virtual bool OnBulletOverlapsDestructible(CallbackContext context, ArcadeColliderType second, ArcadeColliderType first)
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	protected virtual bool OnBulletOverlapsWall(CallbackContext context, ArcadeColliderType bullet, ArcadeColliderType tile)
	{
		//IL_0024: Expected I, but got O
		//IL_0068: Expected I, but got O
		//IL_009b: Expected I4, but got O
		//IL_009b: Expected I4, but got O
		//IL_009f: Expected I, but got O
		//IL_00d6: Expected I, but got I8
		//IL_00e3: Expected I, but got I8
		bool flag = (long)new string("") != 0L;
		Weapon weapon = default(Weapon);
		nint num = (nint)weapon;
		if (!flag)
		{
			Console.WriteLine("");
			Console.WriteLine("砀开瘀㠀\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀匀琀愀最攀");
		}
		bool flag2 = bullet == null;
		ArcadeColliderType arcadeColliderType = tile;
		ArcadeColliderType arcadeColliderType2 = bullet;
		nint num2 = (nint)context;
		nint num3 = num;
		IntPtr intPtr = default(IntPtr);
		if (!flag2)
		{
			Console.WriteLine("漀爀猀⸀伀戀樀攀挀琀猀⸀䐀攀猀琀爀甀挀琀椀戀氀攀㸀⤀Ⰰ\u2000洀攀琀栀漀搀䤀渀昀漀\u2000䀀\u2000爀搀砀Ⰰ\u2000瘀㈀㈀\u2000䀀\u2000爀㠀Ⰰ\u2000瘀㈀㌀");
			num = (nint)new PhaserTile((int)"", (int)"");
			Console.WriteLine("Method not found @180002A60");
			bool flag3 = intPtr == (IntPtr)0;
			arcadeColliderType = tile;
			arcadeColliderType2 = bullet;
			Console.WriteLine("渀椀琀礀⸀倀爀漀昀椀氀椀渀最⸀倀爀漀昀椀氀攀爀䴀愀爀欀攀爀⤀\0門Unmanaged ");
			num2 = 1;
			num3 = 3;
			if (flag3)
			{
			}
		}
		bool flag4;
		ArcadeColliderType arcadeColliderType5 = default(ArcadeColliderType);
		do
		{
			Console.WriteLine("Method not found @180AB7550");
			ArcadeColliderType arcadeColliderType3 = arcadeColliderType;
			ArcadeColliderType arcadeColliderType4 = arcadeColliderType2;
			IntPtr intPtr2 = num2;
			Console.WriteLine("Method not found @1830406E0");
			flag4 = arcadeColliderType5 == null;
			arcadeColliderType = arcadeColliderType5;
			arcadeColliderType2 = bullet;
			num2 = 0;
			num3 = intPtr;
		}
		while (flag4);
		Console.WriteLine("Method not found @180AB7F30");
		bool result = default(bool);
		return result;
	}

	public override void InternalUpdate()
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	public virtual int ActiveProjectileCount()
	{
		//IL_0045: Expected I4, but got I8
		if ((long)_projectilePool == 0L)
		{
			return 0;
		}
		_ = _projectilePool;
		return _projectilePool.countActive(value: true);
	}

	public void AddSpawnedProjectile(Projectile projectile)
	{
		//IL_0093->IL0093: Incompatible stack heights: 1 vs 0
		if ((long)new string("㰀匀礀猀琀攀洀⸀䈀礀琀攀Ⰰ\u2000嘀愀洀瀀椀爀攀匀甀爀瘀") == 0L)
		{
			Console.WriteLine("\u2000爀㤀\0뎁Unknown call target operand");
			Console.WriteLine("");
		}
		bool flag = _spawnedProjectiles == null;
		List<Projectile> spawnedProjectiles = _spawnedProjectiles;
		if (!flag)
		{
			_ = 1L;
			Console.WriteLine("");
			object obj = default(object);
			if (obj != null)
			{
				return;
			}
		}
		Console.WriteLine("Method not found @180AB7550");
	}

	public void DespawnProjectile(Projectile projectile)
	{
		//IL_0090->IL0090: Incompatible stack heights: 1 vs 0
		if ((long)new string("") == 0L)
		{
			Console.WriteLine("搀㨀\u2000嬀瘀㜀㐀\u2000䀀\u2000爀愀砀开瘀\u3100㈀\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀");
			Console.WriteLine("搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀瘀\u3100㘀㜀\u2000䀀\u2000爀愀砀开瘀㈀㔀\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀");
		}
		bool flag = _spawnedProjectiles == null;
		List<object> spawnedProjectiles = (List<object>)(object)_spawnedProjectiles;
		if (!flag)
		{
			_ = 1L;
			Console.WriteLine("");
			object obj = default(object);
			if (obj == null)
			{
				return;
			}
		}
		Console.WriteLine("Method not found @180AB7550");
	}

	public override void Cleanup()
	{
		//IL_003a: Expected O, but got I4
		//IL_0074: Expected O, but got I4
		//IL_00ae: Expected O, but got I4
		//IL_00e8: Expected O, but got I4
		//IL_0138: Expected O, but got I4
		//IL_0122: Expected O, but got I4
		//IL_003a->IL003a: Incompatible stack heights: 2 vs 0
		//IL_0074->IL0074: Incompatible stack heights: 2 vs 0
		//IL_00ae->IL00ae: Incompatible stack heights: 2 vs 0
		//IL_00e8->IL00e8: Incompatible stack heights: 2 vs 0
		//IL_0122->IL0122: Incompatible stack heights: 2 vs 0
		if (_firingAnimEvent != null)
		{
			_ = _firingAnimEvent;
			Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.TimerSystem.Timer::Cancel");
			_ = _firingAnimEvent;
			((Timer)0).Cancel();
		}
		if (_firingTimer != null)
		{
			_ = _firingTimer;
			Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.TimerSystem.Timer::Cancel");
			_ = _firingTimer;
			((Timer)0).Cancel();
		}
		if (_lastShotTimer != null)
		{
			_ = _lastShotTimer;
			Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.TimerSystem.Timer::Cancel");
			_ = _lastShotTimer;
			((Timer)0).Cancel();
		}
		if (_projectilePool != null)
		{
			_ = _projectilePool;
			Console.WriteLine("Unknown operand: Method: VampireSurvivors.Objects.Pools.BulletPool::Cleanup");
			_ = _projectilePool;
			((BulletPool)0).Cleanup();
		}
		if (_secondaryPool != null)
		{
			_ = _secondaryPool;
			Console.WriteLine("Unknown operand: Method: VampireSurvivors.Objects.Pools.BulletPool::Cleanup");
			_ = _secondaryPool;
			((BulletPool)0).Cleanup();
		}
		GameObject gameObject = ((Component)0).gameObject;
		if ((object)gameObject != null)
		{
			Console.WriteLine("Unknown operand: Method: UnityEngine.GameObject::SetActive");
			gameObject.SetActive(value: false);
		}
		else
		{
			Console.WriteLine("Method not found @180AB7550");
		}
	}

	public Vector2 GetPlayerCurrentDirection()
	{
		Vector2 result = default(Vector2);
		if (new string("") != null)
		{
			return result;
		}
		Console.WriteLine("Method not found @180AB7550");
		Vector2 result2 = default(Vector2);
		return result2;
	}

	public virtual bool LevelUp()
	{
		//IL_001f: Expected I4, but got O
		ProfilerMarker markerCleanup = _markerCleanup;
		Console.WriteLine("最愀琀攀⤀Ⰰ\u2000瘀\u3100㐀\u3000\u2000䀀\u2000爀㠀开瘀㠀Ⰰ\u2000\u3000\0啿渀洀愀渀愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀瘀\u3100㌀");
		return (byte)(int)new string("瘀㈀㠀\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀䔀渀攀洀礀䤀渀猀琀愀渀琀椀") != 0;
	}

	public void EnableAdept()
	{
		//IL_000f: Expected I4, but got I8
		//IL_0039: Unknown result type (might be due to invalid IL or missing references)
		//IL_003e: Expected F4, but got Unknown
		IsAdept = true;
		if (_currentWeaponData != null)
		{
			float <interval>k__BackingField = ((WeaponData)(object)this).<interval>k__BackingField * new string("Unmanaged memory load: [188A102CC]");
			WeaponData weaponData = default(WeaponData);
			weaponData.<interval>k__BackingField = <interval>k__BackingField;
		}
		else
		{
			Console.WriteLine("Method not found @180AB7550");
		}
	}

	public override bool LevelUp(bool skipFire)
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	public virtual void HandlePlayerTeleport(float2 destinationPos)
	{
	}

	public virtual float PArea()
	{
		object obj = new string("");
		bool flag = new string("") == null;
		object obj3 = default(object);
		object obj2 = obj3;
		if (!flag)
		{
			object obj4 = obj;
			object obj5 = new string("");
			Console.WriteLine("");
			bool flag2 = _currentWeaponData == null;
			obj2 = new string("");
			if (flag2)
			{
			}
		}
		Console.WriteLine("Method not found @180AB7550");
		float result = default(float);
		return result;
	}

	public virtual int PBounces()
	{
		return _bonusBounces + _bounces;
	}

	public virtual float PAmount()
	{
		//IL_0062: Invalid comparison between O and F4
		//IL_0079: Expected F4, but got O
		//IL_00cb: Unknown result type (might be due to invalid IL or missing references)
		//IL_00d0: Expected F4, but got Unknown
		object obj = new string("");
		bool flag = new string("") == null;
		object obj3 = default(object);
		object obj2 = obj3;
		float num = default(float);
		if (!flag)
		{
			object obj4 = obj;
			object obj5 = new string("");
			Console.WriteLine("䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀⸀䄀挀");
			bool flag2 = System.Runtime.CompilerServices.Unsafe.As<string, UIntPtr>(ref new string("Unmanaged memory load: [188A10B0C]")) <= System.Runtime.CompilerServices.Unsafe.As<object, UIntPtr>(ref (object)num);
			float num2 = (float)new string("Unmanaged memory load: [188A10B0C]");
			if (flag2)
			{
				bool flag3 = _currentWeaponData == null;
				obj2 = new string("");
				if (!flag3)
				{
					return ((WeaponData)(object)this).<amount>k__BackingField + num2;
				}
			}
		}
		Console.WriteLine("Method not found @180AB7550");
		return num;
	}

	public virtual float SecondaryPAmount()
	{
		ProfilerMarker markerCleanup = _markerCleanup;
		object obj = new string("礀\u2000氀漀愀搀㨀\u2000嬀瘀㐀\u3000\u3000\u2000䀀\u2000爀挀砀开瘀㈀㤀\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀䔀渀攀");
		Console.WriteLine("椀猀琀怀\u3100㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀唀䤀⸀䌀甀猀琀漀洀䐀爀漀瀀搀漀");
		float result = default(float);
		return result;
	}

	public virtual float PPower()
	{
		//IL_0056: Expected O, but got I4
		//IL_0074: Expected O, but got I4
		//IL_008b->IL008b: Incompatible stack heights: 3 vs 0
		//IL_008b->IL008b: Incompatible stack heights: 3 vs 0
		bool flag = new string("") == null;
		object obj2 = default(object);
		object obj = obj2;
		VampireSurvivors.Objects.Characters.CharacterController characterController = (VampireSurvivors.Objects.Characters.CharacterController)(object)new string("");
		if (!flag)
		{
			new string("");
			new string("");
			float num = ((VampireSurvivors.Objects.Characters.CharacterController)0).PPowerFinal();
			bool flag2 = _currentWeaponData == null;
			obj = 0;
			characterController = (VampireSurvivors.Objects.Characters.CharacterController)(object)new string("");
			if (flag2)
			{
			}
		}
		Console.WriteLine("Method not found @180AB7550");
		float result = default(float);
		return result;
	}

	public virtual float SecondaryPPower()
	{
		//IL_0056: Expected O, but got I4
		//IL_0074: Expected O, but got I4
		//IL_008b->IL008b: Incompatible stack heights: 3 vs 0
		//IL_008b->IL008b: Incompatible stack heights: 3 vs 0
		bool flag = new string("") == null;
		object obj2 = default(object);
		object obj = obj2;
		VampireSurvivors.Objects.Characters.CharacterController characterController = (VampireSurvivors.Objects.Characters.CharacterController)(object)new string("");
		if (!flag)
		{
			new string("");
			new string("");
			float num = ((VampireSurvivors.Objects.Characters.CharacterController)0).PPowerFinal();
			bool flag2 = _currentWeaponData == null;
			obj = 0;
			characterController = (VampireSurvivors.Objects.Characters.CharacterController)(object)new string("");
			if (flag2)
			{
			}
		}
		Console.WriteLine("Method not found @180AB7550");
		float result = default(float);
		return result;
	}

	public virtual float SecondaryCursePPower()
	{
		//IL_0056: Expected O, but got I4
		//IL_0074: Expected O, but got I4
		//IL_008b->IL008b: Incompatible stack heights: 3 vs 0
		//IL_008b->IL008b: Incompatible stack heights: 3 vs 0
		bool flag = new string("") == null;
		object obj2 = default(object);
		object obj = obj2;
		VampireSurvivors.Objects.Characters.CharacterController characterController = (VampireSurvivors.Objects.Characters.CharacterController)(object)new string("");
		if (!flag)
		{
			new string("");
			new string("");
			float num = ((VampireSurvivors.Objects.Characters.CharacterController)0).PPowerFinal();
			bool flag2 = _currentWeaponData == null;
			obj = 0;
			characterController = (VampireSurvivors.Objects.Characters.CharacterController)(object)new string("");
			if (flag2)
			{
			}
		}
		Console.WriteLine("Method not found @180AB7550");
		float result = default(float);
		return result;
	}

	public virtual float PSpeed()
	{
		//IL_0046: Expected I, but got O
		//IL_0071: Expected I, but got O
		//IL_0082: Expected O, but got I
		//IL_00a8: Invalid comparison between O and F4
		//IL_00bf: Expected F4, but got O
		//IL_010c: Expected I, but got O
		//IL_011e: Expected I, but got O
		//IL_01ab: Expected I, but got O
		//IL_0241: Expected I, but got O
		//IL_02d9: Expected I, but got O
		//IL_032e: Expected O, but got I4
		//IL_0038->IL0038: Incompatible stack heights: 1 vs 0
		//IL_01a2->IL01a2: Incompatible stack heights: 1 vs 0
		//IL_0212->IL0212: Incompatible stack heights: 1 vs 0
		if ((long)new string("") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 17 Call \"il2cpp_codegen_initialize_runtime_metadata\", v16 @ rax_v1, typeof(UnityEngine.Object), methodInfo @ rdx, v18 @ r8, v19 @ r9");
			_ = 1L;
		}
		nint num = (nint)new string("");
		bool flag = new string("") == null;
		object obj2 = default(object);
		object obj = obj2;
		nint num2 = (nint)new string("");
		float num3 = default(float);
		if (!flag)
		{
			object obj3 = num;
			object obj4 = new string("礀\u2000氀漀愀搀㨀\u2000嬀瘀\u3100\u3100㘀\u2000䀀\u2000爀愀砀开瘀\u3100㔀\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀");
			Console.WriteLine("Indirect call: 37 IndirectCall v42 @ rax_v4 (should have been resolved before IL gen)");
			bool flag2 = System.Runtime.CompilerServices.Unsafe.As<string, UIntPtr>(ref new string("Unmanaged memory load: [188A10A10]")) <= System.Runtime.CompilerServices.Unsafe.As<object, UIntPtr>(ref (object)num3);
			float num4 = (float)new string("Unmanaged memory load: [188A10A10]");
			if (flag2)
			{
				bool flag3 = _currentWeaponData == null;
				obj = new string("开爀甀渀琀椀洀攀开挀氀愀猀猀开椀渀椀琀开愀挀琀甀愀氀∀Ⰰ\u2000瘀\u3100㈀㤀\u2000䀀\u2000爀愀砀开瘀㘀Ⰰ\u2000琀礀瀀攀漀昀⠀嘀愀洀瀀椀");
				num2 = (nint)new string("");
				if (!flag3)
				{
					nint num5 = (nint)new UnityEngine.Object();
					float num6 = num4 * ((WeaponData)(object)this).<speed>k__BackingField;
					object obj5 = new string("");
					if ((long)new string("甀洀戀攀爀䴀愀渀愀最攀爀⤀Ⰰ\u2000瘀㐀㘀\u2000䀀\u2000爀搀砀开瘀㈀\u2000⠀匀礀猀琀攀洀⸀䤀渀琀") == 0L)
					{
						Console.WriteLine("最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀瘀\u3100㌀\u3000\u2000䀀\u2000爀愀砀开瘀㠀\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀");
					}
					if ((long)new string("Unmanaged memory load: [189999590]") == 0L)
					{
						Console.WriteLine("椀渀椀琀开愀挀琀甀愀氀∀Ⰰ\u2000瘀\u3100㐀㘀\u2000䀀\u2000爀愀砀开瘀\u3100㘀Ⰰ\u2000琀礀瀀攀漀昀⠀嘀愀洀瀀椀爀攀匀甀爀");
						_ = 1L;
					}
					nint num7 = (nint)new UnityEngine.Object();
					if ((long)new string("㐀㜀\u2000䀀\u2000爀愀砀开瘀\u3100㠀\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀") == 0L)
					{
						Console.WriteLine("");
					}
					if ((long)new string("Unmanaged memory load: [189999551]") == 0L)
					{
						Console.WriteLine("\0\ua980Unmanaged memory load: [v47 @ rdx_v6 (System.Int32");
						_ = 1L;
					}
					bool flag4 = new string("") == null;
					float result = num6;
					if (!flag4)
					{
						nint num8 = (nint)new UnityEngine.Object();
						if ((long)new string("攀挀琀䌀愀氀氀\u2000瘀㈀㐀\u3100㐀\u2000䀀\u2000爀愀砀开瘀㤀㜀\u2000⠀猀栀漀甀氀搀\u2000栀愀瘀攀\u2000戀攀攀渀\u2000爀攀猀漀氀瘀攀搀\u2000戀攀昀漀爀攀") != 0L)
						{
							bool flag5 = (long)new string("") == 0L;
							result = num6;
							if (!flag5)
							{
								object obj6 = new string("");
								bool flag6 = new string("") == null;
								obj = new string("开爀甀渀琀椀洀攀开挀氀愀猀猀开椀渀椀琀开愀挀琀甀愀氀∀Ⰰ\u2000瘀\u3100㈀㤀\u2000䀀\u2000爀愀砀开瘀㘀Ⰰ\u2000琀礀瀀攀漀昀⠀嘀愀洀瀀椀");
								num2 = (nint)new UnityEngine.Object();
								if (flag6)
								{
									goto IL_00c8;
								}
								bool flag7 = (long)new string("") == 0L;
								result = num6;
								if (!flag7)
								{
									new string("");
									new string("");
									float value = ((SineBonus)0).Value;
									float num9 = num6 * num3;
									result = num9;
									Console.WriteLine("Warning: Method ends with non empty stack (-38), the output could be wrong!");
									/*Error: End of method reached without returning.*/;
								}
							}
						}
					}
					return result;
				}
			}
		}
		goto IL_00c8;
		IL_00c8:
		Console.WriteLine("Method not found @180AB7550");
		return num3;
	}

	public virtual float PHitBoxDelayOverSpeed()
	{
		//IL_006e: Expected O, but got I4
		//IL_007f: Expected F4, but got O
		ProfilerMarker markerCleanup = _markerCleanup;
		object obj = new string("");
		Console.WriteLine("砀开瘀㔀崀\0\udb80Warning: Branch target block not in cfg: 7");
		string text = new string("Unmanaged memory load: [188A0FE74]");
		object obj2 = default(object);
		bool flag = System.Runtime.CompilerServices.Unsafe.As<string, UIntPtr>(ref text) >= System.Runtime.CompilerServices.Unsafe.As<object, UIntPtr>(ref obj2);
		object obj3 = new string("Unmanaged memory load: [188A0FE74]");
		if (!flag)
		{
			obj3 = obj2;
		}
		float hitBoxDelay = ((Weapon)0).HitBoxDelay;
		return obj2 * obj3;
	}

	public virtual float PSpeedRepeatInterval()
	{
		//IL_0030: Invalid comparison between O and F4
		//IL_0047: Expected F4, but got O
		//IL_0080: Unknown result type (might be due to invalid IL or missing references)
		//IL_0085: Expected O, but got Unknown
		//IL_008f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0094: Expected F4, but got Unknown
		ProfilerMarker markerCleanup = _markerCleanup;
		object obj = new string("");
		Console.WriteLine("");
		float num = default(float);
		bool flag = System.Runtime.CompilerServices.Unsafe.As<string, UIntPtr>(ref new string("Unmanaged memory load: [188A0FE74]")) >= System.Runtime.CompilerServices.Unsafe.As<object, UIntPtr>(ref (object)num);
		float num2 = (float)new string("Unmanaged memory load: [188A0FE74]");
		if (!flag)
		{
			num2 = num;
		}
		if (_currentWeaponData != null)
		{
			object obj2 = new string("Unmanaged memory load: [188A104DC]") / num2;
			return obj2 * ((WeaponData)(object)this).<repeatInterval>k__BackingField;
		}
		Console.WriteLine("Method not found @180AB7550");
		return num;
	}

	public virtual float PInterval()
	{
		//IL_0086: Expected F4, but got O
		//IL_00bc: Invalid comparison between O and F4
		//IL_00d0: Expected F4, but got O
		//IL_0148: Invalid comparison between F4 and O
		//IL_015f: Expected F4, but got O
		//IL_016d: Expected F4, but got O
		object obj = default(object);
		object obj3 = default(object);
		float result = default(float);
		float num = default(float);
		object obj5 = default(object);
		if ((HasCooldownSpeedBonus ? 1 : 0) != 0L)
		{
			obj = new string("");
			bool flag = new string("") == null;
			object obj2 = obj3;
			result = num;
			object obj4 = obj5;
			object obj6 = new string("");
			if (flag)
			{
				goto IL_00d9;
			}
		}
		float num2 = num;
		float num3 = (float)new string("Unmanaged memory load: [188A104DC]");
		object obj7 = obj5;
		object obj8 = obj;
		object obj9 = new string("");
		Console.WriteLine("");
		bool flag2 = System.Runtime.CompilerServices.Unsafe.As<string, UIntPtr>(ref new string("Unmanaged memory load: [188A104DC]")) > System.Runtime.CompilerServices.Unsafe.As<object, UIntPtr>(ref (object)num);
		float num4 = (float)new string("Unmanaged memory load: [188A104DC]");
		if (flag2)
		{
			while (true)
			{
				IL_018c:
				num2 = num4;
				num3 = num4;
				obj7 = new string("⤀Ⰰ\u2000瘀㈀\u3100\u2000䀀\u2000爀㠀\u2000⠀匀礀猀琀攀洀⸀唀䤀渀琀㌀㈀⤀Ⰰ\u2000瘀㈀㈀\u2000䀀\u2000爀㤀\0㎂Unknown call targe");
				while (true)
				{
					object obj10 = new string("");
					bool flag3 = new string("") == null;
					object obj2 = obj3;
					result = num2;
					object obj4 = obj7;
					object obj6 = new string("");
					if (flag3)
					{
						break;
					}
					string text = new string("Unmanaged memory load: [188A10A10]");
					bool flag4 = System.Runtime.CompilerServices.Unsafe.As<object, UIntPtr>(ref (object)num) <= System.Runtime.CompilerServices.Unsafe.As<string, UIntPtr>(ref text);
					num4 = (float)new string("Unmanaged memory load: [188A10A10]");
					num2 = (float)new string("Unmanaged memory load: [188A10A10]");
					num3 = num;
					obj7 = new string("⤀Ⰰ\u2000瘀㈀\u3100\u2000䀀\u2000爀㠀\u2000⠀匀礀猀琀攀洀⸀唀䤀渀琀㌀㈀⤀Ⰰ\u2000瘀㈀㈀\u2000䀀\u2000爀㤀\0㎂Unknown call targe");
					if (flag4)
					{
						continue;
					}
					goto IL_018c;
				}
				break;
			}
		}
		goto IL_00d9;
		IL_00d9:
		Console.WriteLine("Method not found @180AB7550");
		return result;
	}

	public virtual float PDuration()
	{
		//IL_0041: Expected I, but got O
		//IL_00bf: Expected I, but got O
		//IL_0180: Expected I, but got O
		//IL_01ab: Expected I, but got O
		//IL_014d: Expected I, but got O
		//IL_01e8: Expected O, but got I
		//IL_0267: Invalid comparison between O and F4
		//IL_027e: Expected F4, but got O
		//IL_02c7: Expected O, but got I4
		//IL_0038->IL0038: Incompatible stack heights: 1 vs 0
		//IL_00b6->IL00b6: Incompatible stack heights: 1 vs 0
		//IL_0126->IL0126: Incompatible stack heights: 1 vs 0
		if ((long)new string("漀渀⤀Ⰰ\u2000攀渀攀洀礀\u2000䀀\u2000爀搀砀\u2000⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀䌀栀愀爀愀挀琀攀爀") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 18 Call \"il2cpp_codegen_initialize_runtime_metadata\", v18 @ rax_v1, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
			_ = 1L;
		}
		nint num = (nint)new UnityEngine.Object();
		object obj = new string("");
		if ((long)new string("Unmanaged memory load: [v28 @ rcx_v3 (Il2CppClass<UnityEngine.Object>)+E4]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 35 Call \"il2cpp_runtime_class_init_actual\", v41 @ rax_v3, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
		}
		if ((long)new string("Unmanaged memory load: [189999590]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 49 Call \"il2cpp_codegen_initialize_runtime_metadata\", v55 @ rax_v5, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
			_ = 1L;
		}
		nint num2 = (nint)new UnityEngine.Object();
		if ((long)new string("Unmanaged memory load: [v60 @ rcx_v6 (Il2CppClass<UnityEngine.Object>)+E4]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 65 Call \"il2cpp_runtime_class_init_actual\", v72 @ rax_v7, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
		}
		if ((long)new string("Unmanaged memory load: [189999551]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 79 Call \"il2cpp_codegen_initialize_runtime_metadata\", v86 @ rax_v9, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
			_ = 1L;
		}
		if (new string("") != null)
		{
			nint num3 = (nint)new UnityEngine.Object();
			if ((long)new string("Unmanaged memory load: [v103 @ rcx_v10 (Il2CppClass<UnityEngine.Object>)+E4]") != 0L)
			{
				goto IL_01be;
			}
		}
		goto IL_0172;
		IL_0172:
		nint num4 = (nint)new string("");
		bool flag = new string("") == null;
		object obj3 = default(object);
		object obj2 = obj3;
		nint num5 = (nint)new string("");
		if (!flag)
		{
			Console.WriteLine("");
			goto IL_01be;
		}
		Console.WriteLine("Method not found @180AB7550");
		float num6 = default(float);
		return num6;
		IL_01be:
		if ((long)new string("") != 0L)
		{
			object obj4 = num4;
			object obj5 = new string("渀\u2000挀愀氀氀\u2000琀愀爀最攀琀\u2000漀瀀攀爀愀渀搀㨀\u2000㔀㘀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开爀甀渀琀椀洀攀开挀氀愀猀猀开椀渀椀琀开愀");
			Console.WriteLine("椀洀攀开挀氀愀猀猀开椀渀椀琀开愀挀琀甀愀氀∀Ⰰ\u2000瘀㘀㌀\u2000䀀\u2000爀愀砀开瘀㜀Ⰰ\u2000琀礀瀀攀漀昀⠀唀渀椀琀礀䔀渀最椀渀攀⸀伀");
			float num7 = num6;
			bool flag2 = System.Runtime.CompilerServices.Unsafe.As<string, UIntPtr>(ref new string("Unmanaged memory load: [188A10A10]")) <= System.Runtime.CompilerServices.Unsafe.As<object, UIntPtr>(ref (object)num7);
			float num8 = (float)new string("Unmanaged memory load: [188A10A10]");
			if (!flag2)
			{
				if ((long)new string("") == 0L)
				{
					goto IL_0172;
				}
				num8 = num7;
			}
			float duration = ((Weapon)0).Duration;
			return num7 * num8;
		}
		goto IL_0172;
	}

	public virtual void ParadoxFire()
	{
		ProfilerMarker markerCleanup = _markerCleanup;
		Console.WriteLine("伀戀樀攀挀琀猀⸀䌀栀愀爀愀挀琀攀爀猀⸀䔀渀攀洀");
	}

	public virtual void Fire()
	{
		ProfilerMarker markerCleanup = _markerCleanup;
		Console.WriteLine("伀戀樀攀挀琀猀⸀䌀栀愀爀愀挀琀攀爀猀⸀䔀渀攀洀");
	}

	public virtual void Fire(bool skipTriggers = false)
	{
		//IL_008e: Expected I, but got O
		//IL_0049: Expected O, but got I8
		//IL_00a4: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a9: Expected O, but got Unknown
		//IL_00c0: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c5: Expected O, but got Unknown
		//IL_00f0: Expected O, but got I4
		//IL_0138: Invalid comparison between F4 and O
		//IL_0152: Expected O, but got I4
		//IL_015c: Expected I4, but got O
		//IL_021b: Expected I, but got O
		//IL_022a: Expected O, but got F4
		//IL_026a: Expected I4, but got O
		//IL_02b8: Expected I4, but got I8
		//IL_02c6: Expected I4, but got O
		//IL_02cf: Expected O, but got I4
		//IL_02e1: Expected I, but got O
		//IL_0332: Expected I4, but got O
		//IL_0395: Expected I, but got O
		//IL_03c2: Expected O, but got I4
		//IL_03d0: Expected I, but got O
		//IL_0165->IL0165: Incompatible stack heights: 3 vs 0
		//IL_0387->IL0165: Incompatible stack heights: 3 vs 0
		//IL_03d9->IL0165: Incompatible stack heights: 3 vs 0
		bool flag = (long)new string("渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㜀\u2000䀀\u2000爀愀砀开瘀㌀Ⰰ\u2000琀礀瀀攀") != 0L;
		bool flag3 = default(bool);
		bool flag2 = flag3;
		object obj = default(object);
		if (!flag)
		{
			Console.WriteLine("\u2000䀀\u2000爀愀砀开瘀㌀Ⰰ\u2000琀礀瀀攀漀昀⠀嘀愀洀瀀椀爀攀匀甀");
			obj = (flag3 ? 1 : 0) | 0L;
			Console.WriteLine("椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀䌀栀愀爀愀挀琀攀爀猀⸀䔀渀攀洀椀攀猀⸀䔀渀攀洀礀䨀攀渀攀瘀椀瘀㸀⤀⬀\u3100");
		}
		bool flag4 = new string("") == null;
		bool flag5 = skipTriggers;
		Action<float> action2 = default(Action<float>);
		Action<float> action = action2;
		Transform transform2 = default(Transform);
		Transform transform = transform2;
		nint num = (nint)new string("");
		if (!flag4)
		{
			object obj2 = obj | 0L;
			Console.WriteLine("渀洀愀渀愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀瘀㈀㜀㜀\u2000䀀\u2000爀搀砀开瘀㌀\u2000⠀䤀氀㈀䌀");
			object obj3 = obj2 | 0L;
			Console.WriteLine("䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀䌀栀愀爀愀挀琀攀爀猀⸀䔀渀攀洀椀攀猀⸀䔀渀攀洀礀䨀");
			new string("");
			new string("");
			float2 position = ((ArcadeSprite)skipTriggers).position;
			ProfilerMarker markerCleanup = _markerCleanup;
			Console.WriteLine("");
			ProfilerMarker markerCleanup2 = _markerCleanup;
			object obj4 = new string("栀愀爀愀挀琀攀爀猀⸀䔀渀攀洀礀䌀漀渀琀爀漀氀氀攀爀⤀Ⰰ\u2000瘀㘀\u3000\u2000䀀");
			Console.WriteLine("");
			string text = new string("Unmanaged memory load: [188A104DC]");
			float num2 = default(float);
			bool flag6 = System.Runtime.CompilerServices.Unsafe.As<object, UIntPtr>(ref (object)num2) <= System.Runtime.CompilerServices.Unsafe.As<string, UIntPtr>(ref text);
			float num3 = num2;
			Action<float> action3 = (Action<float>)0;
			bool flag7 = (byte)(int)_targetTransform != 0;
			if (flag6)
			{
				ProfilerMarker markerCleanup3 = _markerCleanup;
				object obj5 = new string("");
				Console.WriteLine("\u2000挀愀氀氀\u2000琀愀爀最攀琀\u2000漀瀀攀爀愀渀搀㨀\u2000㔀㌀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开挀漀搀攀最攀渀开椀渀椀琀椀愀氀椀稀");
				nint num4 = (nint)new Mathf();
				object obj6 = _lastFiringInterval - num3;
				object obj7 = new string("");
				object obj8 = obj6 & (object)new string("Unmanaged memory load: [188A12890]");
				bool flag8 = System.Runtime.CompilerServices.Unsafe.As<object, UIntPtr>(ref obj8) <= System.Runtime.CompilerServices.Unsafe.As<object, UIntPtr>(ref obj7);
				bool flag9 = (byte)(int)new string("") != 0;
				bool flag19 = default(bool);
				Action<float> action6 = default(Action<float>);
				Transform targetTransform = default(Transform);
				nint num5 = default(nint);
				if (!flag8)
				{
					bool flag11 = default(bool);
					bool flag10 = flag11;
					bool flag13 = default(bool);
					bool flag12 = flag13;
					bool flag15 = default(bool);
					bool flag14 = flag15;
					object obj10 = default(object);
					object obj9 = obj10;
					bool flag17 = default(bool);
					bool flag16 = flag17;
					Action action5 = default(Action);
					Action action4 = action5;
					object obj12 = default(object);
					object obj11 = obj12;
					bool flag18 = true;
					flag19 = (byte)(int)new string("䄀㌀䐀㌀㠀崀\0\ue981Unknown call target ") != 0;
					action6 = (Action<float>)0;
					targetTransform = _targetTransform;
					Weapon weapon = default(Weapon);
					num5 = (nint)weapon;
					ProfilerMarker markerCleanup4 = _markerCleanup;
					object obj13 = new string("洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀\u3100㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀礀瀀攀漀昀⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀");
					Console.WriteLine("");
					ProfilerMarker markerCleanup5 = _markerCleanup;
					Weapon weapon2 = default(Weapon);
					weapon2._lastFiringInterval = num3;
					Console.WriteLine("䌀栀愀渀最攀搀⤀\0ﶁUnknown call target operand: 20 Call \"il");
					flag9 = (byte)(int)new string("猀⸀䘀爀愀洀攀眀漀爀欀⸀匀漀甀渀搀䴀愀渀愀最攀爀⤀Ⰰ\u2000洀攀琀栀漀搀䤀渀昀漀\u2000䀀\u2000爀搀砀\u2000⠀匀礀猀琀攀洀⸀匀椀渀最氀") != 0;
				}
				if (skipTriggers)
				{
					return;
				}
				bool flag20 = _currentWeaponData == null;
				flag5 = flag19;
				action = action6;
				transform = targetTransform;
				num = num5;
				if (!flag20)
				{
					nint num6 = (nint)new string("");
					bool flag21 = new string("") == null;
					flag5 = flag9;
					action = action3;
					transform = (Transform)flag7;
					num = (nint)new string("");
					if (!flag21)
					{
						return;
					}
				}
			}
		}
		Console.WriteLine("Method not found @180AB7550");
	}

	public virtual Projectile FireOneProjectile(Vector2 pos, int index, Transform target = null, BulletPool pool = null)
	{
		//IL_0042: Expected I, but got O
		//IL_00d9->IL00d9: Incompatible stack heights: 1 vs 0
		//IL_00d9->IL00d9: Incompatible stack heights: 1 vs 0
		if ((long)new string("") == 0L)
		{
			Console.WriteLine("㠀崀\0啵渀洀愀渀愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀");
			Console.WriteLine("洀愀渀愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀瘀\u3100\u3000㘀㤀\u2000䀀\u2000爀㤀开瘀㔀⬀\u3100㌀\u3000崀\0鮀Unmanaged mem");
		}
		nint num = (nint)new GM();
		List<CharacterType> validStageCharacters = Stage._validStageCharacters;
		bool flag = Stage._validStageCharacters == null;
		Transform transform = target;
		int num2 = index;
		Vector2 vector = pos;
		Stage stage = (Stage)(object)new string("Unmanaged memory load: [v35 @ rax_v4 (Il2CppClass<VampireSurvivors.Framework.GM>)+B8]");
		if (!flag)
		{
			_ = 1L;
			bool flag2 = new string("") == null;
			transform = target;
			num2 = index;
			vector = pos;
			stage = (Stage)(object)new string("");
			if (flag2)
			{
			}
		}
		Console.WriteLine("Method not found @180AB7550");
		Projectile result = default(Projectile);
		return result;
	}

	public virtual Projectile FireOneProjectileIgnoreDistanceToPlayer(Vector2 pos, int index, Transform target = null, BulletPool pool = null)
	{
		//IL_00cb: Expected I, but got O
		//IL_014a: Expected I, but got O
		//IL_01d2: Expected I, but got O
		//IL_0231: Expected I, but got O
		//IL_02a1: Expected I, but got O
		//IL_0378: Expected I4, but got O
		//IL_0329: Expected I, but got O
		//IL_03fa: Expected I4, but got O
		//IL_0038->IL0038: Incompatible stack heights: 1 vs 0
		//IL_00a1->IL00f0: Incompatible stack heights: 0 vs 2
		//IL_0141->IL0141: Incompatible stack heights: 3 vs 2
		//IL_01b1->IL01b1: Incompatible stack heights: 3 vs 2
		//IL_0298->IL0298: Incompatible stack heights: 3 vs 2
		//IL_0308->IL0308: Incompatible stack heights: 3 vs 2
		if ((long)new string("") == 0L)
		{
			Console.WriteLine("");
			_ = 1L;
		}
		BulletPool bulletPool = default(BulletPool);
		bool flag = bulletPool != null;
		BulletPool bulletPool2 = bulletPool;
		if (!flag)
		{
			bool flag2 = _projectilePool == null;
			bulletPool2 = _projectilePool;
			BulletPool projectilePool = _projectilePool;
			if (flag2)
			{
				goto IL_00f0;
			}
		}
		float2 pos2 = default(float2);
		Weapon weapon = default(Weapon);
		Projectile projectile = bulletPool2.SpawnAt(pos2, weapon, index);
		nint num = (nint)new UnityEngine.Object();
		if ((long)new string("") == 0L)
		{
			goto IL_00f0;
		}
		if ((long)new string("Unmanaged memory load: [189999590]") == 0L)
		{
			Console.WriteLine("Ⰰ\u2000瘀㈀㐀\u2000䀀\u2000爀愀砀开瘀㈀Ⰰ\u2000琀");
			_ = 1L;
		}
		nint num2 = (nint)new UnityEngine.Object();
		if ((long)new string("") == 0L)
		{
			Console.WriteLine("䈀㠀崀\0ꎀUnmanaged memory");
		}
		if ((long)new string("Unmanaged memory load: [189999551]") == 0L)
		{
			Console.WriteLine("礀攀爀伀瀀琀椀漀渀猀⤀⬀㤀\u3000崀\0궀Unmanaged memory load: [v82 @ r");
			_ = 1L;
		}
		if ((object)projectile != null)
		{
			nint num3 = (nint)new UnityEngine.Object();
			if ((long)new string("开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀㤀\u2000䀀\u2000爀愀砀开瘀\u3100㌀Ⰰ\u2000洀攀琀栀漀搀漀昀⠀匀礀猀琀攀") != 0L && (long)new string("\u2000漀瀀攀爀愀渀搀㨀\u2000㈀㔀㔀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开爀甀渀琀椀洀攀开挀氀愀猀猀开椀渀椀琀开愀挀琀甀愀氀∀Ⰰ\u2000瘀㌀\u3000") != 0L)
			{
				nint num4 = (nint)new UnityEngine.Object();
				if ((long)new string("") == 0L)
				{
					Console.WriteLine("爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀倀氀愀礀攀爀伀瀀琀椀漀渀猀⬀伀渀嘀愀氀甀攀䌀栀愀渀最攀搀⤀\0啅渀");
				}
				if ((long)new string("Unmanaged memory load: [189999590]") == 0L)
				{
					Console.WriteLine("琀⤀Ⰰ\u2000洀攀琀栀漀搀䤀渀昀漀\u2000䀀\u2000爀搀砀Ⰰ\u2000瘀\u3100㠀\u2000䀀\u2000爀㠀Ⰰ\u2000瘀\u3100㤀\u2000䀀\u2000爀㤀\0熁Unknown call");
					_ = 1L;
				}
				nint num5 = (nint)new UnityEngine.Object();
				if ((long)new string("") == 0L)
				{
					Console.WriteLine("搀漀昀⠀匀礀猀琀攀洀⸀一甀氀氀愀戀氀攀怀\u3100㰀匀礀猀琀攀洀⸀匀椀渀最氀攀㸀㨀㨀最攀琀开䠀愀猀嘀愀氀甀攀⤀Ⰰ\u2000洀攀");
				}
				if ((long)new string("Unmanaged memory load: [189999551]") == 0L)
				{
					Console.WriteLine("");
					_ = 1L;
				}
				if ((object)target != null)
				{
					nint num6 = (nint)new UnityEngine.Object();
					if ((long)new string("") != 0L)
					{
						goto IL_0382;
					}
				}
				goto IL_034e;
			}
		}
		goto IL_01f7;
		IL_01f7:
		return projectile;
		IL_034e:
		ProfilerMarker markerInitProjectile = Projectile._markerInitProjectile;
		Console.WriteLine("");
		Vector2 vector = (Vector2)new string("");
		int num7 = (int)weapon;
		Console.WriteLine("瀀攀漀昀⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀䘀爀愀洀攀眀漀爀欀⸀䜀䴀⤀Ⰰ\u2000洀攀琀栀漀搀䤀渀昀漀\u2000䀀\u2000爀搀砀\u2000⠀䤀氀㈀䌀瀀");
		goto IL_0382;
		IL_0382:
		if ((long)new string("") != 0L)
		{
			while (true)
			{
				object obj = new string("愀琀愀∀Ⰰ\u2000瘀㈀\u3000\u2000䀀\u2000爀愀砀开瘀㈀Ⰰ\u2000琀礀瀀攀漀昀⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀");
				if (new string("愀琀愀∀Ⰰ\u2000瘀㈀\u3000\u2000䀀\u2000爀愀砀开瘀㈀Ⰰ\u2000琀礀瀀攀漀昀⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀") == null)
				{
					break;
				}
				ProfilerMarker markerInitProjectile2 = Projectile._markerInitProjectile;
				Console.WriteLine("");
				vector = (Vector2)target;
				num7 = (int)new string("");
			}
			goto IL_01f7;
		}
		goto IL_034e;
		IL_00f0:
		Console.WriteLine("Method not found @180AB7550");
		Projectile result = default(Projectile);
		return result;
	}

	public Projectile FireOneBullet(float x, float y, int index, Transform target)
	{
		ProfilerMarker markerCleanup = _markerCleanup;
		object obj = new string("");
		Console.WriteLine("⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀䘀爀愀洀攀眀漀爀欀⸀䜀䴀⤀Ⰰ\u2000瘀\u3100㐀\u2000䀀\u2000爀㠀\u2000⠀䤀氀㈀䌀瀀瀀䌀氀");
		return (Projectile)(object)new string("");
	}

	public virtual void DealDamage(IDamageable other)
	{
		ProfilerMarker markerCleanup = _markerCleanup;
		object obj = new string("");
		Console.WriteLine("");
		ProfilerMarker markerCleanup2 = _markerCleanup;
		object obj2 = new string("㘀㐀\u2000䀀\u2000爀㤀开瘀㈀\u2000⠀匀礀猀琀攀洀⸀匀椀渀最氀攀⤀⬀㈀㠀崀\0궀Unmanaged memory load: [v12");
		Console.WriteLine("漀搀㨀\u2000唀渀椀琀礀䔀渀最椀渀攀⸀吀爀愀渀猀昀漀爀洀㨀㨀猀攀琀开爀椀最栀琀\0疁Unknown call t");
		ProfilerMarker markerCleanup3 = _markerCleanup;
		Console.WriteLine("瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀䘀爀愀洀攀眀漀爀欀⸀䜀䴀㸀⤀⬀䈀㠀崀\0쎁Unknown call");
	}

	public virtual void DealDamageRetaliation(IDamageable other)
	{
		//IL_0066: Expected I, but got O
		//IL_00bf->IL00bf: Incompatible stack heights: 2 vs 0
		if ((long)new string("Ⰰ\u2000瘀㐀㌀㘀\u2000䀀\u2000爀愀砀开瘀\u3100㌀Ⰰ") == 0L)
		{
			Console.WriteLine("椀挀猀⤀Ⰰ\u2000瘀\u3100㈀㔀\u2000䀀\u2000爀搀砀开瘀㔀\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀䘀爀愀洀攀眀漀");
			Console.WriteLine("Unknown call target operand: 22 Call \"il2cpp_codegen_initialize_runtime_metadata\", v27 @ rax_v2, typeof(VampireSurvivors.Interfaces.IDamageable), other @ rdx (VampireSurvivors.Interfaces.IDamageable), v21 @ r8 (VampireSurvivors.Interfaces.IDamageable), v22 @ r9");
		}
		bool flag = new string("") == null;
		object obj2 = default(object);
		object obj = obj2;
		IDamageable damageable2 = default(IDamageable);
		IDamageable damageable = damageable2;
		nint num = (nint)other;
		ArcanaManager arcanaManager = (ArcanaManager)(object)new string("");
		if (!flag)
		{
			_ = 1L;
			float critMul = ArcanaManager.CritMul;
			object obj3 = new string("爀㤀开瘀㌀\u2000⠀匀礀猀琀攀洀⸀匀椀");
			Console.WriteLine("");
			object obj4 = default(object);
			bool flag2 = (nint)obj4 <= 0;
		}
		Console.WriteLine("Method not found @180AB7550");
	}

	public virtual void DealDamage(IDamageable other, float damage)
	{
		//IL_0038->IL0038: Incompatible stack heights: 1 vs 0
		if ((long)new string("最㨀\u2000㌀\u3100㔀\u2000䌀漀渀搀椀琀椀漀渀愀氀䨀甀洀瀀\u2000䀀戀㐀㈀Ⰰ\u2000瘀㔀㐀㈀\u2000䀀\u2000吀䔀䴀倀开瘀㤀\u2000⠀匀礀猀琀攀洀") == 0L)
		{
			Console.WriteLine("");
			_ = 1L;
		}
		object obj = default(object);
		if (obj != null)
		{
			Console.WriteLine("Method not found @180002A60");
			object obj2 = default(object);
			if (obj2 != null)
			{
			}
		}
	}

	public void DamageAllEnemies(float value)
	{
		//IL_005c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0061: Expected O, but got Unknown
		//IL_010c: Expected I, but got O
		//IL_00a0: Unknown result type (might be due to invalid IL or missing references)
		//IL_00a5: Expected O, but got Unknown
		//IL_00bc: Unknown result type (might be due to invalid IL or missing references)
		//IL_00c1: Expected O, but got Unknown
		//IL_00fb: Expected F4, but got O
		//IL_0167: Unknown result type (might be due to invalid IL or missing references)
		//IL_016c: Expected O, but got Unknown
		//IL_0183: Unknown result type (might be due to invalid IL or missing references)
		//IL_0188: Expected O, but got Unknown
		bool flag = (long)new string("爀猀⸀伀戀樀攀挀琀猀⸀倀氀愀礀攀爀伀瀀琀椀漀渀猀⬀伀渀嘀愀氀甀攀䌀栀愀渀最攀搀⤀⬀㤀\u3000崀\0궀Unmanaged") != 0L;
		object obj2 = default(object);
		object obj = obj2;
		object obj3 = default(object);
		if (!flag)
		{
			Console.WriteLine("渀最攀搀⤀⬀㤀\u3000崀\0궀Unmanaged memory load: [v82 @ rax_v10");
			obj3 = obj2 | 0L;
			Console.WriteLine("");
		}
		object obj4 = default(object);
		bool flag2 = (nint)obj4 > 0;
		float num = value;
		object obj5 = obj4;
		object obj7 = default(object);
		if (!flag2)
		{
			object obj6 = obj3 | 0L;
			Console.WriteLine("");
			obj7 = obj6 | 0L;
			Console.WriteLine("椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀匀琀愀最攀猀⸀䈀愀挀欀最爀漀甀渀搀䘀漀猀挀愀爀椀㈀㨀㨀㰀䘀爀攀攀䨀攀渀攀瘀椀瘀㸀戀开开㘀㠀");
			ProfilerMarker markerCleanup = _markerCleanup;
			object obj8 = new string("");
			Console.WriteLine("攀挀琀猀⸀匀琀愀最攀猀⸀䈀愀挀欀最爀漀甀渀搀䘀漀猀挀愀爀椀㈀㨀㨀㰀䘀爀攀攀䨀攀渀攀瘀椀瘀㸀戀开开㘀㠀开\u3100\u3000");
			num = (float)new string("");
			object obj9 = default(object);
			obj5 = obj9;
		}
		nint num2 = (nint)new GM();
		object obj10 = new string("");
		HashSet<object>.Enumerator enumerator = (HashSet<object>.Enumerator)obj10;
		bool flag3 = obj10 == null;
		object obj12 = default(object);
		object obj11 = obj12;
		HashSet<object>.Enumerator enumerator2 = (HashSet<object>.Enumerator)obj10;
		float num3 = num;
		object obj13 = obj10;
		if (!flag3)
		{
			object obj14 = obj7 | 0L;
			Console.WriteLine("㨀㨀㰀䘀爀攀攀䨀攀渀攀瘀椀瘀㸀戀开开㘀㠀开\u3100⤀Ⰰ\u2000");
			object obj15 = obj14 | 0L;
			Console.WriteLine("爀愀㈀䐀⸀倀爀漀䌀愀洀攀爀愀㈀䐀⤀Ⰰ\u2000\u3000Ⰰ\u2000瘀\u3100㠀\u2000䀀\u2000爀㠀Ⰰ\u2000瘀\u3100㤀\u2000䀀\u2000爀㤀\0뎁Unknown ");
			HashSet<object>.Enumerator enumerator3 = (HashSet<object>.Enumerator)new string("漀搀䤀渀昀漀\u2000䀀\u2000爀搀砀Ⰰ\u2000瘀\u3100㠀\u2000䀀\u2000爀㠀Ⰰ\u2000瘀\u3100㤀\u2000䀀\u2000爀㤀\0뎁Unknown call targe");
			bool flag4 = new string("漀搀䤀渀昀漀\u2000䀀\u2000爀搀砀Ⰰ\u2000瘀\u3100㠀\u2000䀀\u2000爀㠀Ⰰ\u2000瘀\u3100㤀\u2000䀀\u2000爀㤀\0뎁Unknown call targe") == null;
			obj11 = obj12;
			enumerator2 = (HashSet<object>.Enumerator)new string("漀搀䤀渀昀漀\u2000䀀\u2000爀搀砀Ⰰ\u2000瘀\u3100㠀\u2000䀀\u2000爀㠀Ⰰ\u2000瘀\u3100㤀\u2000䀀\u2000爀㤀\0뎁Unknown call targe");
			num3 = num;
			obj13 = obj10;
			if (flag4)
			{
			}
		}
		Console.WriteLine("Method not found @180AB7550");
	}

	public virtual void StandardCritical(ArcadeColliderType second, ArcadeColliderType first)
	{
		//IL_0024: Expected I, but got O
		//IL_0060: Expected I, but got O
		//IL_010e: Expected I, but got O
		//IL_0142: Expected I, but got O
		//IL_008e: Expected I, but got O
		//IL_00a4: Expected I, but got O
		//IL_0154: Expected O, but got I4
		//IL_0174: Expected O, but got I8
		//IL_0182: Expected I, but got O
		//IL_019f: Expected O, but got I4
		//IL_01db: Expected I, but got O
		//IL_0217: Expected I, but got O
		//IL_0239: Expected I, but got O
		//IL_0243: Expected I, but got O
		//IL_00cf->IL014b: Incompatible stack heights: 0 vs 1
		//IL_01ec->IL00cf: Incompatible stack heights: 1 vs 0
		//IL_024c->IL00cf: Incompatible stack heights: 1 vs 0
		bool flag = (long)new string("") != 0L;
		Weapon weapon = default(Weapon);
		nint num = (nint)weapon;
		if (!flag)
		{
			Console.WriteLine("");
			Console.WriteLine("");
		}
		bool flag2 = first == null;
		ArcadeColliderType arcadeColliderType = first;
		nint num2 = (nint)second;
		nint num3 = num;
		ArcadeColliderType arcadeColliderType2 = default(ArcadeColliderType);
		nint num6;
		if (!flag2)
		{
			Console.WriteLine("洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀匀琀愀最攀猀⸀䈀愀挀欀最爀漀甀渀搀䘀漀猀挀愀爀椀㈀");
			Console.WriteLine("愀最攀猀⸀䈀愀挀欀最爀漀甀渀搀䘀漀猀挀愀爀椀㈀⬀㰀㸀挀⤀Ⰰ\u2000洀攀琀栀漀搀䤀渀昀漀\u2000䀀\u2000爀搀砀Ⰰ\u2000瘀\u3100㠀\u2000䀀\u2000爀㠀Ⰰ\u2000");
			nint num4 = (nint)new EnemyController();
			arcadeColliderType2 = first;
			nint num5 = (nint)new string("");
			string text = new string("\u3100㤀㈀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀");
			string text2 = new string("");
			bool flag3 = System.Runtime.CompilerServices.Unsafe.As<string, UIntPtr>(ref text) < System.Runtime.CompilerServices.Unsafe.As<string, UIntPtr>(ref text2);
			num6 = num;
			if (flag3)
			{
				goto IL_014b;
			}
		}
		goto IL_00cf;
		IL_00cf:
		Console.WriteLine("Method not found @180AB7550");
		ArcadeColliderType arcadeColliderType3 = arcadeColliderType;
		IntPtr intPtr = num2;
		IntPtr intPtr2 = num3;
		Console.WriteLine("渀琀愀椀渀猀⤀Ⰰ\u2000\u3000Ⰰ\u2000\u3000Ⰰ\u2000瘀\u3100㤀\u2000䀀\u2000爀㤀\0薀Unmanaged memory load: [v153 @");
		_ = 1L;
		num = (nint)new Projectile();
		object obj = new string("");
		bool flag4 = (object)new string("氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀䘀爀愀洀攀眀漀爀") != new EnemyController();
		num6 = (nint)new string("");
		if (flag4)
		{
		}
		goto IL_014b;
		IL_014b:
		object obj2 = 0;
		nint num7 = num6;
		Console.WriteLine("Method not found @180AB7560");
		obj2 = 1L;
		num7 = (nint)new string("");
		bool flag5 = obj2 == null;
		ArcadeColliderType arcadeColliderType4 = (ArcadeColliderType)0;
		if (!flag5)
		{
			_ = 0;
			System.ThrowHelper.ThrowArgumentOutOfRange_IndexException();
			return;
		}
		bool flag6 = arcadeColliderType4 == null;
		arcadeColliderType = arcadeColliderType2;
		num2 = (nint)new EnemyController();
		num3 = num7;
		if (!flag6)
		{
			if (new string("") != null)
			{
				return;
			}
			nint num8 = (nint)_critChancesArray;
			bool flag7 = _critChancesArray == null;
			arcadeColliderType = arcadeColliderType2;
			num2 = (nint)new EnemyController();
			num3 = (nint)_critChancesArray;
			if (!flag7)
			{
				return;
			}
		}
		goto IL_00cf;
	}

	public override void CheckArcanas()
	{
		//IL_001c: Expected O, but got I4
		Console.WriteLine("\u2000挀愀氀氀\u2000琀愀爀最攀琀\u2000漀瀀攀爀愀渀搀㨀\u2000㐀㤀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开挀漀搀攀最攀渀开椀渀椀琀椀愀氀椀稀");
		((Weapon)0).CheckBeginningArcana();
	}

	public void CheckBeginningArcana()
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	public bool HasActiveArcanaOfType(ArcanaType arcanaType)
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	public bool CheckFreeze()
	{
		//IL_0041: Expected I, but got O
		//IL_00bf: Expected I, but got O
		//IL_014d: Expected I, but got O
		//IL_0219: Expected I4, but got O
		//IL_0038->IL0038: Incompatible stack heights: 1 vs 0
		//IL_00b6->IL00b6: Incompatible stack heights: 1 vs 0
		//IL_0126->IL0126: Incompatible stack heights: 1 vs 0
		if ((long)new string("") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 18 Call \"il2cpp_codegen_initialize_runtime_metadata\", v18 @ rax_v1, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
			_ = 1L;
		}
		nint num = (nint)new UnityEngine.Object();
		object obj = new string("");
		if ((long)new string("Unmanaged memory load: [v28 @ rcx_v3 (Il2CppClass<UnityEngine.Object>)+E4]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 35 Call \"il2cpp_runtime_class_init_actual\", v41 @ rax_v3, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
		}
		if ((long)new string("Unmanaged memory load: [18999958F]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 49 Call \"il2cpp_codegen_initialize_runtime_metadata\", v55 @ rax_v5, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
			_ = 1L;
		}
		nint num2 = (nint)new UnityEngine.Object();
		if ((long)new string("Unmanaged memory load: [v60 @ rcx_v6 (Il2CppClass<UnityEngine.Object>)+E4]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 65 Call \"il2cpp_runtime_class_init_actual\", v72 @ rax_v7, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
		}
		if ((long)new string("Unmanaged memory load: [189999551]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 79 Call \"il2cpp_codegen_initialize_runtime_metadata\", v86 @ rax_v9, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
			_ = 1L;
		}
		if (new string("") != null)
		{
			nint num3 = (nint)new UnityEngine.Object();
			if ((long)new string("Unmanaged memory load: [v101 @ rcx_v10 (Il2CppClass<UnityEngine.Object>)+E4]") != 0L && (long)new string("") != 0L)
			{
				ProfilerMarker markerCleanup = _markerCleanup;
				object obj2 = new string("眀渀\u2000挀愀氀氀\u2000琀愀爀最攀琀\u2000漀瀀攀爀愀渀搀㨀\u2000㐀㜀㜀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开爀甀渀琀椀洀攀开挀氀愀猀猀开");
				Console.WriteLine("");
				object obj3 = new string("");
				if (new string("") != null)
				{
					object obj4 = obj3;
					Console.WriteLine("礀猀琀攀洀⸀䄀挀琀椀漀渀⤀Ⰰ\u2000\u3000Ⰰ\u2000\u3000\0\ue180Unmanaged memory loa");
					Console.WriteLine("Not implemented instruction: seta al");
					return (byte)(int)new string("琀猀⸀匀琀愀最攀猀⸀䈀愀挀欀最爀漀甀渀搀䘀漀猀挀愀爀椀㈀⬀㰀㸀挀㸀⤀⬀䈀㠀崀\0啕渀洀愀渀愀最攀搀\u2000") != 0;
				}
				Console.WriteLine("Method not found @180AB7550");
				bool result = default(bool);
				return result;
			}
		}
		return false;
	}

	public bool CheckDefang()
	{
		//IL_0041: Expected I, but got O
		//IL_00bf: Expected I, but got O
		//IL_014d: Expected I, but got O
		//IL_023b: Expected I4, but got O
		//IL_0038->IL0038: Incompatible stack heights: 1 vs 0
		//IL_00b6->IL00b6: Incompatible stack heights: 1 vs 0
		//IL_0126->IL0126: Incompatible stack heights: 1 vs 0
		if ((long)new string("") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 18 Call \"il2cpp_codegen_initialize_runtime_metadata\", v18 @ rax_v1, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
			_ = 1L;
		}
		nint num = (nint)new UnityEngine.Object();
		object obj = new string("");
		if ((long)new string("Unmanaged memory load: [v28 @ rcx_v3 (Il2CppClass<UnityEngine.Object>)+E4]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 35 Call \"il2cpp_runtime_class_init_actual\", v41 @ rax_v3, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
		}
		if ((long)new string("Unmanaged memory load: [18999958F]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 49 Call \"il2cpp_codegen_initialize_runtime_metadata\", v55 @ rax_v5, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
			_ = 1L;
		}
		nint num2 = (nint)new UnityEngine.Object();
		if ((long)new string("Unmanaged memory load: [v60 @ rcx_v6 (Il2CppClass<UnityEngine.Object>)+E4]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 65 Call \"il2cpp_runtime_class_init_actual\", v72 @ rax_v7, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
		}
		if ((long)new string("Unmanaged memory load: [189999551]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 79 Call \"il2cpp_codegen_initialize_runtime_metadata\", v86 @ rax_v9, typeof(UnityEngine.Object), methodInfo @ rdx, v20 @ r8, v21 @ r9");
			_ = 1L;
		}
		if (new string("") != null)
		{
			nint num3 = (nint)new UnityEngine.Object();
			if ((long)new string("Unmanaged memory load: [v101 @ rcx_v10 (Il2CppClass<UnityEngine.Object>)+E4]") != 0L && (long)new string("") != 0L)
			{
				ProfilerMarker markerCleanup = _markerCleanup;
				object obj2 = new string("眀渀\u2000挀愀氀氀\u2000琀愀爀最攀琀\u2000漀瀀攀爀愀渀搀㨀\u2000㐀㜀㜀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开爀甀渀琀椀洀攀开挀氀愀猀猀开");
				Console.WriteLine("");
				ProfilerMarker markerCleanup2 = _markerCleanup;
				object obj3 = new string("渀愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀瘀㘀\u3000㤀\u2000䀀\u2000爀愀砀开瘀㔀㘀⬀㔀\u3000崀\0\ue180Unmanage");
				Console.WriteLine("㘀\u2000⠀䤀氀㈀䌀瀀瀀䌀氀愀猀猀㰀嘀愀洀瀀椀爀攀匀甀爀瘀");
				object obj4 = new string("");
				if (new string("") != null)
				{
					object obj5 = obj4;
					Console.WriteLine("琀愀爀最攀琀\u2000漀瀀攀爀愀渀搀㨀\u2000");
					Console.WriteLine("Not implemented instruction: seta al");
					return (byte)(int)new string("") != 0;
				}
				Console.WriteLine("Method not found @180AB7550");
				bool result = default(bool);
				return result;
			}
		}
		return false;
	}

	public virtual void CopyAccumulatedLimitBreaks(Weapon from, Weapon to)
	{
		bool flag = (object)from == null;
		Weapon weapon = to;
		Weapon weapon2 = from;
		Weapon weapon4 = default(Weapon);
		Weapon weapon3 = weapon4;
		if (!flag)
		{
			bool flag2 = (object)to == null;
			weapon = to;
			weapon2 = from;
			weapon3 = (Weapon)(object)accumulatedLimitBreaks;
			if (flag2)
			{
			}
		}
		Console.WriteLine("Method not found @180AB7550");
	}

	public virtual bool ApplyLimitBreak(WeightedLimitBreak weightedLimitBreak)
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	public virtual Projectile SpawnExplosionAt(float2 pos, int enemiesHit = 0, int damage = 1, float area = 1f)
	{
		//IL_0077: Expected O, but got I4
		//IL_0044: Unknown result type (might be due to invalid IL or missing references)
		//IL_0049: Expected O, but got Unknown
		//IL_0124: Expected I4, but got O
		//IL_0095: Unknown result type (might be due to invalid IL or missing references)
		//IL_009a: Expected O, but got Unknown
		//IL_00b1: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b6: Expected O, but got Unknown
		//IL_0103->IL017c: Incompatible stack heights: 1 vs 0
		//IL_0103->IL0103: Incompatible stack heights: 1 vs 0
		//IL_017c->IL017c: Incompatible stack heights: 2 vs 0
		bool flag = (long)new string("甀渀搀䘀漀猀挀愀爀椀㈀⬀㰀㸀挀㸀⤀Ⰰ\u2000瘀㔀㜀㌀\u2000䀀\u2000爀愀砀开瘀㔀㈀\u2000⠀匀礀猀琀攀洀⸀䄀挀琀椀漀渀⤀Ⰰ\u2000\u3000Ⰰ\u2000\u3000") != 0L;
		ArcadePhysicsCallback arcadePhysicsCallback2 = default(ArcadePhysicsCallback);
		ArcadePhysicsCallback arcadePhysicsCallback = arcadePhysicsCallback2;
		object obj = default(object);
		if (!flag)
		{
			Console.WriteLine("");
			obj = arcadePhysicsCallback2 | 0L;
			Console.WriteLine("");
		}
		bool flag2 = _secondaryPool != null;
		ArcadePhysicsCallback arcadePhysicsCallback4 = default(ArcadePhysicsCallback);
		ArcadePhysicsCallback arcadePhysicsCallback3 = arcadePhysicsCallback4;
		ArcadeColliderType arcadeColliderType = (ArcadeColliderType)enemiesHit;
		ArcadePhysicsCallback arcadePhysicsCallback6;
		int num;
		float2 float5;
		ProjectileFactory projectileFactory;
		if (!flag2)
		{
			object obj2 = obj | 0L;
			Console.WriteLine("");
			ArcadePhysicsCallback arcadePhysicsCallback5 = (ArcadePhysicsCallback)(obj2 | 0L);
			_ = 1L;
			arcadePhysicsCallback = arcadePhysicsCallback5;
			bool flag3 = (object)_projectileFactory == null;
			arcadePhysicsCallback6 = arcadePhysicsCallback4;
			num = enemiesHit;
			float5 = pos;
			projectileFactory = _projectileFactory;
			if (flag3)
			{
				goto IL_017c;
			}
		}
		bool flag4 = _secondaryPool == null;
		arcadePhysicsCallback6 = arcadePhysicsCallback3;
		num = (int)arcadeColliderType;
		float5 = pos;
		projectileFactory = (ProjectileFactory)(object)_secondaryPool;
		if (!flag4)
		{
			_ = _projectileFactory;
			Projectile projectilePrefab = _projectileFactory.GetProjectilePrefab(_explosionType);
			ProfilerMarker markerCleanup = _markerCleanup;
			Console.WriteLine("匀礀猀琀攀洀⸀䄀挀琀椀漀渀⤀Ⰰ\u2000\u3000Ⰰ\u2000\u3000");
			Console.WriteLine("挀㸀⤀⬀䈀㠀崀\0㎁Unknown call target op");
		}
		goto IL_017c;
		IL_017c:
		Console.WriteLine("Method not found @180AB7550");
		Projectile result = default(Projectile);
		return result;
	}

	public virtual void ResetFiringTimer()
	{
		//IL_0049: Expected O, but got I8
		//IL_0082: Unknown result type (might be due to invalid IL or missing references)
		//IL_0087: Expected I4, but got Unknown
		//IL_00b8: Expected O, but got I4
		//IL_00c1: Expected O, but got I4
		//IL_024e: Expected F4, but got O
		//IL_0287: Expected I4, but got O
		//IL_0287: Expected O, but got I4
		//IL_0287: Expected I4, but got O
		//IL_0287: Expected O, but got I8
		//IL_0287: Expected O, but got I4
		//IL_0287: Expected F4, but got O
		//IL_01f2: Expected I, but got O
		//IL_02ef: Expected F4, but got O
		//IL_0328: Expected I4, but got O
		//IL_0328: Expected O, but got I4
		//IL_0328: Expected I4, but got O
		//IL_0328: Expected O, but got I8
		//IL_0328: Expected O, but got I4
		//IL_0328: Expected F4, but got O
		//IL_0368: Unknown result type (might be due to invalid IL or missing references)
		//IL_036d: Expected O, but got Unknown
		//IL_039a: Unknown result type (might be due to invalid IL or missing references)
		//IL_039f: Expected O, but got Unknown
		//IL_00c1->IL00c1: Incompatible stack heights: 3 vs 0
		//IL_01f2->IL01f2: Incompatible stack heights: 1 vs 0
		//IL_035b->IL03b1: Incompatible stack heights: 3 vs 2
		//IL_03b1->IL03b1: Incompatible stack heights: 3 vs 2
		bool flag = (long)new string("") != 0L;
		bool flag2 = default(bool);
		bool isLooped = flag2;
		object obj = default(object);
		if (!flag)
		{
			Console.WriteLine("Unknown call target operand: 17 Call \"il2cpp_codegen_initialize_runtime_metadata\", v16 @ rax_v1, typeof(System.Action), methodInfo @ rdx, v18 @ r8, v19 @ r9");
			obj = (flag2 ? 1 : 0) | 0L;
			Console.WriteLine("");
		}
		bool flag3 = _firingTimer == null;
		object obj3 = default(object);
		object obj2 = obj3;
		if (!flag3)
		{
			bool flag4 = (byte)(int)(obj | 0L) != 0;
			_ = 1L;
			isLooped = flag4;
			_ = _firingTimer;
			Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.TimerSystem.Timer::Cancel");
			_ = _firingTimer;
			((Timer)0).Cancel();
			obj2 = 0;
		}
		if (_currentWeaponData == null || (((WeaponData)(object)this).<intervalDependsOnDuration>k__BackingField ? 1 : 0) != 0L)
		{
			Console.WriteLine("Method not found @180AB7550");
			return;
		}
		ProfilerMarker markerCleanup = _markerCleanup;
		object obj4 = new string("");
		Console.WriteLine("");
		object obj6 = default(object);
		object obj5 = obj6;
		ProfilerMarker markerCleanup2 = _markerCleanup;
		Console.WriteLine("爀瘀椀瘀漀爀猀⸀伀戀樀攀挀琀猀⸀匀琀愀最攀猀⸀䈀愀挀欀最爀漀甀渀搀䘀漀猀挀愀爀椀㈀㨀㨀㰀匀琀愀爀琀匀瀀愀眀渀椀渀最倀");
		Action action = default(Action);
		Weapon weapon = default(Weapon);
		if (new string("眀渀\u2000挀愀氀氀\u2000琀愀爀最攀琀\u2000漀瀀攀爀愀渀搀㨀\u2000㌀㤀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开瘀洀开漀戀樀攀挀琀开渀攀眀∀Ⰰ") == null)
		{
			Console.WriteLine("");
			ProfilerMarker markerCleanup3 = _markerCleanup;
			Console.WriteLine("Unknown operand: Method: System.Action::.ctor");
			action..ctor(weapon, (IntPtr)new string(""));
		}
		Console.WriteLine("⬀㠀⬀瘀㔀㤀\u2000䀀\u2000爀搀砀开瘀㔀崀\0\udb80Warning");
		Console.WriteLine("Unknown operand: Method: System.Action::.ctor");
		Action action2 = default(Action);
		action2..ctor(weapon, (IntPtr)0);
		ProfilerMarker markerCleanup4 = _markerCleanup;
		object obj7 = new string("");
		Console.WriteLine("⠀匀礀猀琀攀洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䰀椀猀琀怀\u3100㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀");
		float num = obj5 * (object)new string("Unmanaged memory load: [188A0FE74]");
		MonoBehaviour monoBehaviour = default(MonoBehaviour);
		int num2 = default(int);
		TimerType repeat = default(TimerType);
		Timer firingTimer = Timers.Register((float)action, (Action)0, (Action<float>)1L, isLooped, (byte)(int)monoBehaviour != 0, (MonoBehaviour)num2, (int)repeat, TimerType.GAME, (byte)(int)weapon != 0, canPause: false);
		bool flag5 = (long)new string("Unmanaged memory load: [18996C9E0]") == 0L;
		Weapon weapon2 = default(Weapon);
		weapon2._firingTimer = firingTimer;
		if (!flag5)
		{
			ProfilerMarker markerCleanup5 = _markerCleanup;
			object obj8 = new string("");
			Console.WriteLine("");
			float num3 = obj5 * (object)new string("Unmanaged memory load: [188A0FE74]");
			Timer firingTimer2 = Timers.Register((float)action2, (Action)0, (Action<float>)1L, isLooped, (byte)(int)monoBehaviour != 0, (MonoBehaviour)num2, (int)repeat, TimerType.GAME, (byte)(int)weapon != 0, canPause: false);
			bool flag6 = (long)new string("Unmanaged memory load: [18996C9E0]") == 0L;
			weapon2._firingTimer = firingTimer2;
			if (!flag6)
			{
				object obj9 = weapon + 176L;
				object obj10 = new string("Unmanaged memory load: [1899A9880]");
				object obj11 = obj9 >> 12;
				object obj12 = obj11 & 0x1FFFFFL;
				object obj13 = obj12 >> 6;
			}
		}
	}

	protected void FireAndQueueAnimation()
	{
		//IL_001c: Expected O, but got I4
		Console.WriteLine("");
		((Weapon)0).PlayNextAttackAnim();
		ProfilerMarker markerCleanup = _markerCleanup;
		Console.WriteLine("");
	}

	protected void PlayNextAttackAnim()
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	protected virtual FiringAnimation GetFiringAnimation()
	{
		return FiringAnimation.None;
	}

	public void RemoveFiringTimer()
	{
		//IL_003a: Expected O, but got I4
		//IL_0074: Expected O, but got I4
		//IL_003a->IL003a: Incompatible stack heights: 2 vs 0
		if (_firingTimer != null)
		{
			_ = _firingTimer;
			Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.TimerSystem.Timer::Cancel");
			_ = _firingTimer;
			((Timer)0).Cancel();
		}
		if (_firingAnimEvent != null)
		{
			_ = _firingAnimEvent;
			Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.TimerSystem.Timer::Cancel");
			_ = _firingAnimEvent;
			((Timer)0).Cancel();
		}
	}

	public virtual void SetVisible(bool visible)
	{
		_isVisible = visible;
	}

	public static List<float> MakeChanceArray(int amount = 100)
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	protected virtual float CalcCritMul()
	{
		//IL_00b3: Expected F4, but got O
		//IL_00a8->IL00a8: Incompatible stack heights: 1 vs 0
		if ((long)new string("") == 0L)
		{
			Console.WriteLine("");
			Console.WriteLine("戀氀攀\u2000琀漀\u2000甀猀攀爀∀Ⰰ\u2000洀攀琀栀漀搀䤀渀昀漀\u2000䀀\u2000爀搀砀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀㠀Ⰰ\u2000瘀㈀㜀\u2000䀀\u2000");
		}
		if ((<CanCrit>k__BackingField ? 1 : 0) != 0L)
		{
			Console.WriteLine("");
			_ = 1L;
			List<float> critChancesArray = _critChancesArray;
			bool flag = _critChancesArray == null;
			float num = default(float);
			float result = num;
			object obj2 = default(object);
			object obj = obj2;
			List<float> critChancesArray2 = _critChancesArray;
			if (flag)
			{
				Console.WriteLine("Method not found @180AB7550");
				return result;
			}
		}
		return (float)new string("Unmanaged memory load: [188A104DC]");
	}

	public virtual float GetChanceFromArray()
	{
		//IL_0060: Expected I4, but got O
		//IL_0087: Expected I4, but got O
		//IL_00ec: Expected O, but got I4
		//IL_00a7: Expected O, but got I4
		//IL_00bf: Expected O, but got I4
		//IL_00fa: Expected O, but got I4
		//IL_00c8->IL00c8: Incompatible stack heights: 1 vs 0
		//IL_0105->IL0105: Incompatible stack heights: 1 vs 0
		if ((long)new string("") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 17 Call \"il2cpp_codegen_initialize_runtime_metadata\", v16 @ rax_v1, methodof(System.Collections.Generic.List`1<System.Single>::get_Count), methodInfo @ rdx, v18 @ r8, v19 @ r9");
			Console.WriteLine("Unknown call target operand: 20 Call \"il2cpp_codegen_initialize_runtime_metadata\", v24 @ rax_v2, methodof(System.Collections.Generic.List`1<System.Single>::get_Item), methodInfo @ rdx, v18 @ r8, v19 @ r9");
		}
		int critIndex = _critIndex;
		List<float> critChancesArray = _critChancesArray;
		Weapon weapon = default(Weapon);
		weapon._critIndex = (int)new string("Unmanaged memory load: [v28 @ rax_v4 (System.Int32)+1]");
		bool flag = _critChancesArray == null;
		object obj2 = default(object);
		object obj = obj2;
		int num = (int)new string("Unmanaged memory load: [v28 @ rax_v4 (System.Int32)+1]");
		object obj5;
		if (!flag)
		{
			_ = 1L;
			object obj3 = critIndex >> 31;
			bool flag2 = (nint)obj3 == 1;
			object obj4 = ~(flag2 ? 1 : 0);
			if (obj4 != null)
			{
				obj5 = 0;
				Console.WriteLine("Method not found @180AB7560");
				goto IL_0105;
			}
		}
		Console.WriteLine("Method not found @180AB7550");
		object obj6 = obj;
		int num2 = num;
		obj5 = -1;
		goto IL_0105;
		IL_0105:
		Console.WriteLine("Not implemented instruction: idiv dword ptr [r8+18h]");
		object obj7 = obj5;
		string text = new string("Unmanaged memory load: [v29 @ r8_v1 (System.Collections.Generic.List`1<System.Single>)+18]");
		if (System.Runtime.CompilerServices.Unsafe.As<object, UIntPtr>(ref obj7) < System.Runtime.CompilerServices.Unsafe.As<string, UIntPtr>(ref text))
		{
		}
		_ = 0;
		System.ThrowHelper.ThrowArgumentOutOfRange_IndexException();
		float result = default(float);
		return result;
	}

	protected override void MakeLevelOne()
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	public void ReloadCurrentData()
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	protected override Dictionary<WeaponType, JArray> GetDataDictionary()
	{
		object obj = new string("");
		if (new string("") != null)
		{
			return (Dictionary<WeaponType, JArray>)(object)new string("Unmanaged memory load: [v0 @ rax_v1+150]");
		}
		Console.WriteLine("Method not found @180AB7550");
		Dictionary<WeaponType, JArray> result = default(Dictionary<WeaponType, JArray>);
		return result;
	}

	private void ApplyLimitBreakStatsToWeaponStats(LimitBreakData limitBreakData)
	{
		if (limitBreakData != null)
		{
			Console.WriteLine("");
			limitBreakData.ApplyDataToWeapon(_currentWeaponData);
		}
	}

	public Weapon()
	{
		/*Error: Empty body found. Decompiled assembly might be a reference assembly.*/;
	}

	static Weapon()
	{
		//IL_0074: Expected I4, but got O
		//IL_0074: Expected O, but got I8
		//IL_0081: Expected I, but got O
		//IL_0097: Expected O, but got I
		//IL_00b8: Expected I4, but got O
		//IL_00b8: Expected O, but got I8
		//IL_00c5: Expected I, but got O
		//IL_0044: Unknown result type (might be due to invalid IL or missing references)
		//IL_0049: Expected O, but got Unknown
		bool flag = (long)new string("") != 0L;
		object obj2 = default(object);
		object obj = obj2;
		if (!flag)
		{
			Console.WriteLine("");
			object obj3 = obj2 | 0L;
			Console.WriteLine("氀漀挀欀\u2000渀漀琀\u2000椀渀\u2000挀昀最㨀\u2000㜀\u3100\u2000䌀漀渀搀椀琀椀漀渀愀氀䨀甀洀瀀\u2000䀀戀\u3100\u3100Ⰰ\u2000瘀\u3100\u3000㈀\u2000䀀\u2000");
		}
		IntPtr intPtr = ProfilerUnsafeUtility.CreateMarker((string)1L, 0, MarkerFlags.Default, (int)obj);
		nint num = (nint)new Weapon();
		object obj4 = new string("漀猀挀愀爀椀㈀㨀㨀䐀攀瘀漀甀爀䔀最最猀\0뎀Unknown operan");
		obj4 = (nint)intPtr;
		IntPtr intPtr2 = ProfilerUnsafeUtility.CreateMarker((string)1L, 0, MarkerFlags.Default, (int)obj);
		nint num2 = (nint)new Weapon();
		object obj5 = new string("渀愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀\u3100㠀㤀㤀䄀㘀㈀㠀㈀崀\0羁Unknown call tar");
	}
}
